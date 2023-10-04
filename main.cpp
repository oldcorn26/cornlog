#include <chrono>
#include <thread>
#include <atomic>
#include <vector>
#include "include/log.h"

const int repeated_time = 8888;
const int thr_cnt = 1;  // 启动4个线程
std::atomic<int> start_cnt(0);

void threadRun(int id) {
    while (start_cnt < thr_cnt) ;  // 等待所有线程都准备好

    for (int i = 0; i < repeated_time; i++) {
        corn::logInfo("ID: {}, Counter: {}", id, i);
    }
}

int main() {
    std::vector<std::thread> threads;
    
    for (int i = 0; i < thr_cnt; i++) {
        threads.emplace_back([i]() {
            start_cnt++;  // 当前线程已准备好
            threadRun(i);  // 开始执行日志任务
        });
    }

    // 记录测试开始时间
    auto start_time = std::chrono::high_resolution_clock::now();

    // 等待所有线程完成
    for (auto& t : threads) {
        t.join();
    }

    // 记录测试结束时间
    auto end_time = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    std::cout << std::format("All threads finished in {} milliseconds, average {} ms\n", duration, (double)duration / repeated_time);

    return 0;
}
