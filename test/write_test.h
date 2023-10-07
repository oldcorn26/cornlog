#include <chrono>
#include <vector>
#include "../include/log.h"

namespace corn {

namespace test {

void writeTest(int repeated_time) {
    int i = 0;
    for (; i < corn::gLogConfig.log_buffer_len; i++) {
        corn::logInfo("Counter:{}", i);
    }

    auto start_time = std::chrono::high_resolution_clock::now();
    for (; i < repeated_time; i++) {
        corn::logInfo("Counter:{}", i);
    }
    auto end_time = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    std::cout << std::format("{} times cost {} milliseconds, average {} ms\n",
                             repeated_time - corn::gLogConfig.log_buffer_len, duration,
                             (double)duration / (repeated_time - corn::gLogConfig.log_buffer_len))
              << std::endl;

    return;
}

} // namespace test

} // namespace corn
