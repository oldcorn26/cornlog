#pragma once

#include <iostream>
#include <string>
#include <source_location>
#include <format>
#include <cstdint>
#include <cstring>
#include <sstream>
#include "log_level.h"
#include "log_config.h"
#include "log_file_appender.h"

namespace corn {

namespace detail {

template<typename T>
class WithSourceLocation {
public:
    template<class U>
    requires std::constructible_from<T, U>
    consteval WithSourceLocation(U &&inner, std::source_location loc = std::source_location::current())
            : inner(std::forward<U>(inner)), loc(loc) {};

    constexpr T const &format() const { return inner; }

    constexpr std::source_location const &location() const { return loc; }

private:
    T inner;
    std::source_location loc;
};

consteval std::size_t compile_time_strlen(const char* str) {
    std::size_t len = 0;
    while (*str++) ++len;
    return len;
}

inline LogFileAppender logFileAppender(gLogConfig.file_option.file_path);

template<LogLevel lev, typename... Args>
void genericLog(detail::WithSourceLocation<std::format_string<Args...>> fmt, Args &&... args) {
    if (lev < gLogConfig.log_level) {
        return;
    }
    Output output = {fmt.location(),
                     std::string(detail::to_output_string(lev)) +
                     std::vformat(fmt.format().get(), std::make_format_args(args...))};
    logFileAppender.append(std::move(output));
}

} // namespace detail

#define FUNCTION(name) \
    template<typename... Args> \
    void log##name(detail::WithSourceLocation<std::format_string<Args...>> fmt, Args &&... args) { \
            return genericLog<LogLevel::name>(std::move(fmt), std::forward<Args>(args)...);\
            }
FOREACH_LOG_LEVEL(FUNCTION)
#undef FUNCTION

} // namespace corn
