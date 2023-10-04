#pragma once

#include <stdint.h>
#include <string>

namespace corn {

enum class FileWriterType : uint8_t {
    MMAPFILE = 0,
    APPENDFILE
};

namespace detail {

class FileWriter {
public:
    FileWriter() = default;
    virtual ~FileWriter() = default;

    virtual void append(const char *msg, int32_t len) = 0;
    virtual void flush() = 0;
    virtual uint32_t writtenBytes() const = 0;
};

class MMapFileWriter : public FileWriter {
public:
    MMapFileWriter(const std::string &basename, uint32_t mem_size);
    ~MMapFileWriter();

    void append(const char *msg, int32_t len) override;
    void flush() override;
    uint32_t writtenBytes() const override;

private:
    int fd_;
    char *buffer_;
    int32_t mem_size_;
    int32_t writed_;
};

class AppendFileWriter : public FileWriter {
public:
    AppendFileWriter(const std::string &filename);
    ~AppendFileWriter();

    void append(const char *msg, int32_t len) override;
    void flush() override;
    uint32_t writtenBytes() const override;

private:
    FILE *fp_;
    char buffer_[64 * 1024];
    uint32_t writen_ = 0;
};

} // namespace detail

} // namespace corn
