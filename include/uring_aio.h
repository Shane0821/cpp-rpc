#ifndef _URING_AIO_H
#define _URING_AIO_H

#include <liburing.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <vector>

template <bool SQ_POLL, bool FD_FIXED, unsigned int QUEUE_DEPTH = 512>
class UringAIO {
   public:
    UringAIO() {
        memset(&params_, 0, sizeof(params_));
        if constexpr (SQ_POLL) {
            params_.flags |= IORING_SETUP_SQPOLL;
            params_.sq_thread_idle = 2000;
        }

        int rc = io_uring_queue_init_params(QUEUE_DEPTH, &ring_, &params_);
        if (rc != 0) {
            throw std::runtime_error(std::string("io_uring_queue_init_params failed: ") +
                                     strerror(-rc));
        }

        if constexpr (SQ_POLL && !FD_FIXED) {
            // If we require fixed files, ensure kernel supports SQPOLL with fixed or
            // non-fixed appropriately. IORING_FEAT_SQPOLL_NONFIXED means SQPOLL works
            // with non-fixed files; if not present and we don't register, raw fds in
            // SQPOLL will fail.
            if (!(params_.features & IORING_FEAT_SQPOLL_NONFIXED)) {
                io_uring_queue_exit(&ring_);
                throw std::runtime_error(
                    "SQPOLL requires fixed files on this kernel; set "
                    "require_fixed_files=true and register files");
            }
        }
    }

    ~UringAIO() { close(); }

    // Register a set of fds as fixed files; returns the count registered.
    bool register_fds(const int* fds, int num) {
        if constexpr (FD_FIXED) {
            if (num <= 0) return false;
            int ret = io_uring_register_files(&ring_, fds, num);
            if (ret < 0) {
                std::cerr << "error registering files: " << strerror(-ret) << std::endl;
                return false;
            }
            registered_files_ = num;
            return true;
        }
        return false;
    }

    // Unregister registered files (if any).
    void unregister_fds() {
        if constexpr (FD_FIXED) {
            if (registered_files_ > 0) {
                int ret = io_uring_unregister_files(&ring_);
                if (ret < 0) {
                    std::cerr << "error unregistering files: " << strerror(-ret)
                              << std::endl;
                }
                registered_files_ = 0;
            }
        }
    }

    // Submit an async write.
    void write_async(const char* data, size_t size, off_t offset, int fd_or_index) {
        if constexpr (FD_FIXED) {
            if (registered_files_ == 0) [[unlikely]] {
                std::cerr << "No files registered but write_async requested fixed file\n";
                return;
            }
        }

        if (pending_ >= QUEUE_DEPTH / 2) peek_completions();

        // Copy buffer so it's alive until completion (could be optimized with
        // user-managed lifetimes).
        auto buf = std::make_unique<char[]>(size);
        std::memcpy(buf.get(), data, size);

        io_uring_sqe* sqe = io_uring_get_sqe(&ring_);
        if (!sqe) [[unlikely]] {
            // wait for completions if queue is full
            wait_sq_space_left();
            sqe = io_uring_get_sqe(&ring_);
            if (!sqe) {
                std::cerr << "failed to get SQE for write" << data << '\n';
                return;
            }
        }

        auto* req = new WriteRequest{std::move(buf), size, offset};

        // Prepare initial write
        io_uring_prep_write(sqe, fd_or_index, req->data.get(), size, offset);
        if constexpr (FD_FIXED) {
            sqe->flags |= IOSQE_FIXED_FILE;
        }
        io_uring_sqe_set_data(sqe, req);

        int ret = io_uring_submit(&ring_);
        if (ret < 0) [[unlikely]] {
            std::cerr << "submit failed: " << strerror(-ret) << std::endl;
            delete req;
            return;
        }

        ++pending_;
    }

    // Submit an fsync on the given fd (fixed or raw) and wait for all pending including
    // this fsync.
    void fsync_and_wait(int fd_or_index, bool data_only = false) {
        io_uring_sqe* sqe = io_uring_get_sqe(&ring_);
        if (!sqe) [[unlikely]] {
            // Ensure we at least wait current completions if queue is full
            wait_sq_space_left();
            sqe = io_uring_get_sqe(&ring_);
            if (!sqe) {
                std::cerr << "failed to get SQE for fsync\n";
                return;
            }
        }
        unsigned flags = data_only ? IORING_FSYNC_DATASYNC : 0;
        io_uring_prep_fsync(sqe, fd_or_index, flags);
        if constexpr (FD_FIXED) {
            sqe->flags |= IOSQE_FIXED_FILE;
        }
        // No user_data needed for fsync; we track via pending_
        io_uring_sqe_set_data(sqe, nullptr);

        int ret = io_uring_submit(&ring_);
        if (ret < 0) [[unlikely]] {
            std::cerr << "submit fsync failed: " << strerror(-ret) << std::endl;
            return;
        }
        ++pending_;

        // Wait for everything outstanding
        ret = io_uring_submit(&ring_);
        if (ret < 0) [[unlikely]] {
            std::cerr << "submit failed: " << strerror(-ret) << std::endl;
        }
        // Drain to clear CQEs
        wait_all();
    }

    void close() {
        if (closed_) return;
        // drain outstanding I/O
        wait_all();
        unregister_fds();
        io_uring_queue_exit(&ring_);
        closed_ = true;
    }

   private:
    struct WriteRequest {
        std::unique_ptr<char[]> data;
        size_t size;
        off_t offset;
    };

    // Wait for at least one completion. Returns true if the operation completed
    // successfully (or all retries eventually did).
    bool wait_for_completion() {
        io_uring_cqe* cqe = nullptr;
        int err = io_uring_wait_cqe(&ring_, &cqe);
        if (err < 0) [[unlikely]] {
            std::cerr << "error waiting for completion: " << strerror(-err) << std::endl;
            return false;
        }
        bool ret = handle_cqe(cqe);
        io_uring_cqe_seen(&ring_, cqe);
        return ret;
    }

    // Wait until all current pending I/Os complete.
    void wait_all() {
        while (pending_ > 0) {
            wait_for_completion();
        }
    }

    // Non-blocking harvesting of completions. Returns number processed.
    void peek_completions() {
        io_uring_cqe* cqe = nullptr;
        while (io_uring_peek_cqe(&ring_, &cqe) == 0) {
            handle_cqe(cqe);
            io_uring_cqe_seen(&ring_, cqe);
        }
    }

    void wait_sq_space_left() {
        while (!io_uring_sq_space_left(&ring_)) {
            peek_completions();
        }
    }

    bool handle_cqe(io_uring_cqe* cqe) {
        // Note: cqe->user_data may be null (e.g., fsync we submitted without data)
        WriteRequest* req = reinterpret_cast<WriteRequest*>(io_uring_cqe_get_data(cqe));

        if (req == nullptr) [[unlikely]] {
            // e.g., fsync completion
            --pending_;
            return true;
        }

        if (cqe->res < 0) [[unlikely]] {
            std::cerr << "Async write failed: " << strerror(-cqe->res) << " for "
                      << req->size << " bytes at offset " << req->offset << std::endl;
            delete req;
            --pending_;
            return false;
        }

        // Full write completed
        delete req;
        --pending_;
        return true;
    }

    io_uring ring_{};
    io_uring_params params_{};
    size_t pending_{0};
    int registered_files_{0};
    bool closed_{false};
};

#endif