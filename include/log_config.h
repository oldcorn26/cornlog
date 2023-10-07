#pragma once

#include <cstdint>
#include <string>
#include "file_writer.h"
#include "log_level.h"

namespace corn {

struct LogConfig {
  uint32_t log_buffer_len = 200;
  uint32_t log_buffer_nums = 2;
  LogLevel log_level = LogLevel::Info;

  struct FileOption {
    std::string file_path = "../log";

    // Log rolling size and interval, exclusive for AsyncFileAppender
    uint32_t log_file_size = 2048 * 1024;
    uint32_t log_file_margin = 1024;
    uint32_t log_flush_file_size;
    uint32_t log_flush_interval;

    FileWriterType file_writer_type = FileWriterType::MMAPFILE;

  } file_option;
};

inline constexpr LogConfig gLogConfig{};

} // namespace corn
