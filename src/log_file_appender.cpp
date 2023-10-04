#include <iostream>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../include/log_file_appender.h"
#include "../include/log.h"
#include "../include/log_buffer.h"

namespace corn {

namespace detail {

LogFileAppender::LogFileAppender(const std::string &basename)
    : started_(false), running_(false), basename_(basename),
      buffer_(std::bind(&LogFileAppender::writeLogs, this, std::placeholders::_1), gLogConfig.log_buffer_len) {
  start();
}

LogFileAppender::~LogFileAppender() {
  if (started_) {
    stop();
  }
}

void LogFileAppender::start() {
    started_ = true;
    running_ = true;
    persist_thread_ = std::thread(&LogFileAppender::threadFunc, this); // 启动线程
}

void LogFileAppender::stop() {
    running_ = false;
    persist_thread_.join();
    started_ = false;
}

void LogFileAppender::initFileWriter() {
    static int fileCount_ = 0;
    std::string fileName = basename_ + std::to_string(fileCount_) + ".txt";
    fileCount_++;
    if (gLogConfig.file_option.file_writer_type == FileWriterType::MMAPFILE) {
        fileWriter_ = std::make_unique<MMapFileWriter>(fileName, gLogConfig.file_option.log_file_size);
    } else if (gLogConfig.file_option.file_writer_type == FileWriterType::APPENDFILE) {
        fileWriter_ = std::make_unique<AppendFileWriter>(fileName);
    }
}

void LogFileAppender::threadFunc() {
    const uint32_t MAX_MEM = gLogConfig.file_option.log_file_size - gLogConfig.file_option.log_file_margin;
    initFileWriter();

    while (running_) {
        buffer_.processLogs();

        if (fileWriter_->writtenBytes() >= MAX_MEM) {
            fileWriter_->flush();
            initFileWriter();
        }
    }
}

void LogFileAppender::writeLogs(std::vector<std::string> logs) {
    for (auto &log : logs) {
        if (log.empty()) continue;
        fileWriter_->append(log.c_str(), log.size());
    }
}

} // namespace detail

} // namespace corn
