#pragma once

#include <atomic>
#include <vector>
#include <string>
#include <mutex>
#include <array>
#include <functional>
#include <condition_variable>

namespace corn {

namespace detail {

class LogBuffer {
public:
    LogBuffer(const std::function<void(std::vector<std::string>)> &callback, size_t max_size)
            : MAX_SIZE(max_size), current_index_(0), current_buffer_(&buffers_[0]), backup_buffer_(&buffers_[1]),
              should_process_(false), callback_(callback) {
        buffers_[0].resize(MAX_SIZE);
        buffers_[1].resize(MAX_SIZE);
    }

    ~LogBuffer() {
        swapBuffers();
        callback_(std::move(*backup_buffer_));
    }

    // multi-producer threads use this interface
    void append(std::string log) {
        size_t idx;
        do {
            idx = current_index_.fetch_add(1, std::memory_order_relaxed);
        } while (idx >= MAX_SIZE);

        current_buffer_->at(idx) = std::move(log);
        if (idx == MAX_SIZE - 1) {
            swapBuffers();
            notifyForProcessing();
        }
    }

    // single-consumer thread use this interface
    void processLogs() {
        std::unique_lock<std::mutex> lock(process_mutex_);
        while (!should_process_) {
            cv_.wait(lock);
        }

        callback_(std::move(*backup_buffer_));
        backup_buffer_->resize(MAX_SIZE);
        should_process_ = false;
    }

private:
    void swapBuffers() {
        std::swap(current_buffer_, backup_buffer_);
        current_index_.store(0, std::memory_order_release);
    }

    void notifyForProcessing() {
        std::unique_lock<std::mutex> lock(process_mutex_);
        should_process_ = true;
        cv_.notify_one();
    }

private:
    const size_t MAX_SIZE;
    std::atomic<size_t> current_index_;
    std::array<std::vector<std::string>, 2> buffers_;
    std::vector<std::string> *current_buffer_;
    std::vector<std::string> *backup_buffer_;
    std::function<void(std::vector<std::string>)> callback_;

    // for process
    std::mutex process_mutex_;
    std::condition_variable cv_;
    bool should_process_;
};

} // namespace detail

} // namespace corn
