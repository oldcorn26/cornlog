#include <chrono>
#include <thread>
#include <atomic>
#include <vector>
#include "../include/log.h"

namespace corn {

namespace test {

void threadRun(std::atomic<int> &thread_flag, int repeated_time) {
    int thread_id = thread_flag.load();
    while (thread_flag > 0) ;
    for (int i = 0; i < repeated_time; i++) {
        corn::logInfo("ID: {}, Counter: {}", thread_id, i);
    }
}

void multiThreadTest(int repeated_time, int thread_cnt) {

    std::vector<std::thread> threads;
    std::atomic<int> thread_flag(thread_cnt);
    int i = 0;

    for (; i < corn::gLogConfig.log_buffer_len; i++) {
        corn::logInfo("Counter:{}", -1);
    }
    
    for (; thread_flag > 0; --thread_flag) {
        threads.emplace_back([&thread_flag, repeated_time]() {
            threadRun(thread_flag, repeated_time);
        });
    }

    auto start_time = std::chrono::high_resolution_clock::now();
    for (auto& t : threads) {
        t.join();
    }
    auto end_time = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    std::cout << std::format("{} threads total {} times cost {} milliseconds, average {} ms\n",
                             thread_cnt, thread_cnt * repeated_time, duration, (double)duration / (thread_cnt * repeated_time))
              << std::endl;

    return;
}

} // namespace test

} // namespace corn
