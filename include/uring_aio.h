#ifndef _URING_AIO_H
#define _URING_AIO_H

#include <fcntl.h>
#include <liburing.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

class UringAIO {
   public:
    UringAIO() {
        if (geteuid()) {
            throw std::runtime_error("you need root privileges to run aio.\n");
        }

        struct io_uring_params params;
        memset(&params, 0, sizeof(params));
        params.flags |= IORING_SETUP_SQPOLL;
        params.sq_thread_idle = 2000;  // 2s

        if (io_uring_queue_init_params(QUEUE_DEPTH, &ring_, &params) != 0) {
            throw std::runtime_error("failed to initialize io_uring");
        }

        // Check if SQPOLL feature is actually supported
        if (!(params.features & IORING_FEAT_SQPOLL_NONFIXED)) {
            io_uring_queue_exit(&ring_);
            throw std::runtime_error("SQPOLL not supported on this system");
        }
    }

    ~UringAIO() {
        close();
        io_uring_queue_exit(&ring_);
    }

    bool register_fds(const int* fds, int num) {
        int ret = io_uring_register_files(&ring_, fds, num);
        if (ret) {
            std::cerr << "Error registering fd: " << strerror(-ret) << std::endl;
            return false;
        }
        return true;
    }

    void write_async(const char* data, size_t size, off_t offset = -1) {
        // Create a copy of the data to ensure it persists until the write completes
        auto data_copy = std::make_unique<char[]>(size);
        std::memcpy(data_copy.get(), data, size);

        // Get an SQE (Submission Queue Entry)
        struct io_uring_sqe* sqe = io_uring_get_sqe(&ring_);

        if (!sqe) {
            // In SQPOLL mode, we might need to check for completions to free up SQEs
            peek_completions();  // Process completions to free SQEs
            sqe = io_uring_get_sqe(&ring_);
            if (!sqe) {
                std::cerr << "failed to get SQE\n";
                return;
            }
        }

        // Prepare write operation
        if (offset == -1) {
            // Append mode. 0 means fd[0].
            io_uring_prep_write(sqe, 0, data_copy.get(), size, -1);
        } else {
            // Specific offset
            io_uring_prep_write(sqe, 0, data_copy.get(), size, offset);
        }
        sqe->flags |= IOSQE_FIXED_FILE;

        // Store the data pointer in user_data to keep it alive
        WriteRequest* request = new WriteRequest{std::move(data_copy), offset, size};

        io_uring_sqe_set_data(sqe, request);

        // Submit the operation
        int submitted = io_uring_submit(&ring_);
        if (submitted < 0) {
            delete request;
            std::cerr << "failed to submit IO: " << std::string(strerror(-submitted))
                      << std::endl;
        }
    }

    bool wait_for_completion() {
        struct io_uring_cqe* cqe;
        // wait until a completion happens
        int ret = io_uring_wait_cqe(&ring_, &cqe);
        if (ret < 0) {
            std::cerr << "Error waiting for completion: " << strerror(-ret) << std::endl;
            return false;
        }

        // Process completion
        bool success = process_completion(cqe);

        // Mark completion as seen
        io_uring_cqe_seen(&ring_, cqe);

        return success;
    }

    bool wait_for_completions(size_t count) {
        bool all_success = true;

        for (size_t i = 0; i < count; ++i) {
            if (!wait_for_completion()) {
                all_success = false;
            }
        }

        return all_success;
    }

    size_t peek_completions() {
        // Check for any available completions without waiting
        struct io_uring_cqe* cqe;
        size_t completed = 0;

        while (io_uring_peek_cqe(&ring_, &cqe) == 0) {
            process_completion(cqe);
            io_uring_cqe_seen(&ring_, cqe);
            completed++;
        }

        return completed;
    }

    void flush() {
        // For SQPOLL mode, we need to ensure all operations are completed
        // before calling fsync
        io_uring_submit_and_wait(&ring_, 0);  // Wait for all pending operations
    }

    void close() {
        flush();
        io_uring_unregister_files(&ring_);
    }

   protected:
    bool process_completion(struct io_uring_cqe* cqe) {
        WriteRequest* request = reinterpret_cast<WriteRequest*>(cqe->user_data);

        if (cqe->res < 0) {
            std::cerr << "Async write failed: " << strerror(-cqe->res) << " for "
                      << request->size << " bytes at offset " << request->offset
                      << std::endl;
        } else if (static_cast<size_t>(cqe->res) != request->size) {
            std::cerr << "Partial write: " << cqe->res << " bytes written instead of "
                      << request->size << " at offset " << request->offset << std::endl;
        }

        // Clean up the request
        delete request;

        return cqe->res >= 0;
    }

    struct WriteRequest {
        std::unique_ptr<char[]> data;
        off_t offset;
        size_t size;
    };

    static constexpr size_t QUEUE_DEPTH{128};

    struct io_uring ring_;
};

// Advanced version with CPU affinity support (Alibaba Cloud Linux specific)
// class AdvancedAsyncFileWriter : public AsyncFileWriter {
//    private:
//     struct io_uring ring_;
//     int fd_;

//    public:
//     AdvancedAsyncFileWriter(int cpu_affinity = -1) : fd_(-1) {
//         struct io_uring_params params = {};
//         params.flags = IORING_SETUP_SQPOLL;

// // Enable percpu sqthread and affinity features if supported
// #ifdef IORING_SETUP_SQPOLL_PERCPU
//         params.flags |= IORING_SETUP_SQPOLL_PERCPU | IORING_SETUP_SQ_AFF;
// #endif

//         if (cpu_affinity >= 0) {
//             params.sq_thread_cpu = cpu_affinity;
//         }

//         params.sq_thread_idle = 100;  // 100ms idle timeout

//         if (io_uring_queue_init_params(32, &ring_, &params) != 0) {
//             throw std::runtime_error(
//                 "Failed to initialize io_uring with advanced SQPOLL");
//         }

//         // Additional initialization for advanced features...
//     }

//     // Rest of the implementation would be similar to the base class
//     // but with additional optimizations for SQPOLL
// };

#endif