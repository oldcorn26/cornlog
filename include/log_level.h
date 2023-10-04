#pragma once

#include <stdint.h>
#include <string>

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

} // namespace detail

} // namespace corn
