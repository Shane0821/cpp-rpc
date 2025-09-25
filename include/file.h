#ifndef _FILE_H
#define _FILE_H

#include "uring_aio.h"

template <bool SQ_POLL, bool FD_FIXED>
class File {
   public:
    File() = default;

    ~File() { close_file(); }

    bool open_file(const std::string &file_path, bool append = false) {
        path_ = file_path;

        int flags = O_WRONLY | O_CREAT;
        if (append) {
            flags |= O_APPEND;
        } else {
            flags |= O_TRUNC;
        }

        fd_ = open(path_.c_str(), flags, S_IRUSR | S_IWUSR);
        if (fd_ == -1) {
            std::cerr << "failed to open file: " << path_ << std::endl;
            return false;
        }

        offset_ = lseek64(fd_, 0, SEEK_END);
        if (offset_ < 0) {
            std::cerr << "failed to seek end: " << offset_ << std::endl;
            return false;
        }

        if constexpr (FD_FIXED == true) {
            return aio_.register_fds(&fd_, 1);
        } else {
            return true;
        }
    }

    void write(const char *data, size_t len) {
        if (fd_ != -1) [[likely]] {
            if constexpr (FD_FIXED == true) {
                aio_.write_async(data, len, offset_, 0);
            } else {
                aio_.write_async(data, len, offset_, fd_);
            }
            offset_ += len;
        }
    }

    void flush() {
        if (fd_ != -1) {
            if constexpr (FD_FIXED == true) {
                aio_.fsync_and_wait(0);
            } else {
                aio_.fsync_and_wait(fd_);
            }
        }
    }

    void close_file() {
        if (fd_ != -1) {
            flush();
            aio_.close();
            close(fd_);
            fd_ = -1;
        }
    }

    int fd() const { return fd_; }
    const std::string &path() const { return path_; }

   private:
    UringAIO<SQ_POLL, FD_FIXED> aio_;
    std::string path_;
    int fd_{-1};
    off_t offset_{-1};
};

#endif