#ifndef LOG_HPP
#define LOG_HPP

#include "LogLevel.hpp"

namespace omf {

class Log {
public:
    static Log& instance(void) {
        static Log instance;
        return instance;
    }

    bool print(LogLevel a_level, const char* a_file, int a_line, const char* a_fmt, ...) {
        if (a_level < m_level) {
            return false;
        }

        va_list args;
        va_start(args, a_fmt);
        vprintf(a_fmt, args);
        va_end(args);
        return true;
    }


private:
    Log() = default;
    Log(const Log&) = delete;
    Log& operator=(const Log&) = delete;
    Log(Log&&) = delete;
    Log& operator=(Log&&) = delete;

    LogLevel m_level = LogLevel::INFO;
};

} // namespace omf

#endif // LOG_HPP