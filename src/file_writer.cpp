#include "../include/file_writer.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <cstring>

namespace corn {

namespace detail {

MMapFileWriter::MMapFileWriter(const std::string &basename, uint32_t mem_size) : mem_size_(mem_size), writed_(0) {
    if (fd_ >= 0) {
        close(fd_);
    }
    fd_ = open(basename.c_str(), O_RDWR | O_CREAT,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd_ < 0) {
        fprintf(stderr, "open new file failed,errno=%d", errno);
    } else {
        int n = ftruncate(fd_, mem_size);
        (void) n;
        buffer_ = (char *) mmap(NULL, mem_size_, PROT_READ | PROT_WRITE,
                                MAP_SHARED, fd_, 0);
        if (buffer_ == MAP_FAILED) {
            fprintf(stderr, "mmap file failed,errno=%d", errno);
        }
    }
}

MMapFileWriter::~MMapFileWriter() {
    if (fd_ >= 0) {
        close(fd_);
        fd_ = -1;
    }
    if (buffer_ != MAP_FAILED) {
        munmap(buffer_, mem_size_);
    }
}

void MMapFileWriter::append(const char *msg, int32_t len) {
    if (len > mem_size_ - writed_) {
        fprintf(stderr, "mmap memory overflow ,errno=%d", errno);
        return;
    }
    memcpy(buffer_ + writed_, msg, len);
    writed_ += len;
}

void MMapFileWriter::flush() {
    if (buffer_ != MAP_FAILED) {
        msync(buffer_, mem_size_, MS_ASYNC);
    }
}

uint32_t MMapFileWriter::writtenBytes() const { return writed_; }


AppendFileWriter::AppendFileWriter(const std::string &filename)
        : fp_(::fopen(filename.c_str(), "ae")) {
    ::setbuffer(fp_, buffer_, sizeof buffer_);
}

AppendFileWriter::~AppendFileWriter() {
    if (fp_) {
        ::fclose(fp_);
    }
}

void AppendFileWriter::append(const char *msg, int32_t len) {
    size_t n = ::fwrite_unlocked(msg, 1, len, fp_);
    size_t remain = len - n;
    while (remain > 0) {
        size_t x = ::fwrite_unlocked(msg + n, 1, remain, fp_);
        if (x == 0) {
            int err = ferror(fp_);
            if (err) {
                fprintf(stderr, "AppendFile::append() failed %s\n", strerror(err));
            }
            break;
        }
        n += x;
        remain = len - n; // remain -= x
    }

    writen_ += len;
}

void AppendFileWriter::flush() { ::fflush(fp_); }

uint32_t AppendFileWriter::writtenBytes() const { return writen_; }


} // namespace detail

} // namespace corn
