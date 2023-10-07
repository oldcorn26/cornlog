#pragma once

#include <vector>
#include <string>
#include <memory>
#include <source_location>
#include "log_buffer.h"
#include "file_writer.h"
#include "log_level.h"

namespace corn {

namespace detail {

class LogFileAppender {
public:
    LogFileAppender(const std::string &basename);

    ~LogFileAppender();

    template<typename T>
    void append(T &&msg) {
        buffer_.append(std::forward<T>(msg));
    }
    
    void start();

    void stop();

private:
    void threadFunc();

    void initFileWriter();

    void writeLogs(std::vector<Output> logs);

    bool started_;
    bool running_;
    std::string basename_;
    LogBuffer<Output> buffer_;
    std::thread persist_thread_;
    std::unique_ptr<FileWriter> fileWriter_;
};

} // namespace detail

} // namespace corn
