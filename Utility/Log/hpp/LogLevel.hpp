#ifndef LOG_LEVEL_HPP
#define LOG_LEVEL_HPP

#include <cstdint>

namespace omf {

enum class LogLevel : std::uint8_t {
    DEBUG = 0,
    INFO = 1,
    WARN = 2,
    ERROR = 3
};

} // namespace omf

#endif // LOG_LEVEL_HPP
