#pragma once

#include <stdint.h>
#include <string>
#include <source_location>

namespace corn {

#define FOREACH_LOG_LEVEL(f) \
    f(Debug) \
    f(Info) \
    f(Warning) \
    f(Error) \
    f(Fatal)

enum class LogLevel : uint8_t {
#define FUNCTION(name) name,
        FOREACH_LOG_LEVEL(FUNCTION)
#undef FUNCTION
};

namespace detail {

inline std::string to_string(LogLevel level) {
#define FUNCTION(name) case LogLevel::name: return #name;
    switch (level) {
        FOREACH_LOG_LEVEL(FUNCTION)
    }
    return "Unknown";
#undef FUNCTION
}
    
inline consteval const char *to_output_string(LogLevel level) {
#define FUNCTION(name) case LogLevel::name: return " ["#name"] ";
    switch (level) {
        FOREACH_LOG_LEVEL(FUNCTION)
    }
    return " [Unknown] ";
#undef FUNCTION
}

using Output = std::pair<std::source_location, std::string>;

} // namespace detail

} // namespace corn
