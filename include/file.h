#ifndef _FILE_H
#define _FILE_H

#include "uring_aio.h"

class File {
   public:
    File() = default;

    ~File() {
        if (fd_) {
            close(fd_);
        }
    }

    bool open_file(const std::string &file_path, bool append = false) {
        path_ = file_path;

        int flags = O_WRONLY | O_CREAT;
        if (append) {
            flags |= O_APPEND;
        } else {
            flags |= O_TRUNC;
        }

        fd_ = open(path_.c_str(), flags, 0644);
        if (fd_ == -1) {
            std::cerr << "failed to open file: " << path_ << std::endl;
            return false;
        }

        return aio_.register_fds(&fd_, 1);
    }

    void write(const char *data, size_t len) { aio_.write_async(data, len, -1); }

    void flush() {
        if (fd_ != -1) {
            aio_.flush();
            fsync(fd_);
        }
    }

    void close_file() {
        if (fd_ != -1) {
            aio_.close();
            close(fd_);
            fd_ = -1;
        }
    }

    int fd() const { return fd_; }
    const std::string &path() const { return path_; }

   private:
    UringAIO aio_;
    std::string path_;
    int fd_{-1};
};

#endif