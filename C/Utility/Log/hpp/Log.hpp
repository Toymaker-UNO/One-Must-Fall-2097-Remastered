#ifndef LOG_HPP
#define LOG_HPP

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>

#ifdef __cplusplus
extern "C" {
#endif

namespace omf {

class Log {
public:
    enum class Level {
        DEBUG = 0,
        INFO = 1,
        WARNING = 2,
        ERROR_LEVEL = 3  // ERROR 매크로와 충돌 방지
    };

    static Log& instance(void) {
        static Log instance;
        return instance;
    }

    void initialize(const char* a_file_paht = "./log.txt") {
        if( NULL != m_file ) {
            return;
        }

        m_file = fopen(a_file_paht, "w+");
    }

    void finalize(void) {
        if( NULL == m_file ) {
            return; 
        }
        
        fclose(m_file);
        m_file = nullptr;
    }

    FILE* get_file(void) {
        return m_file;
    }

    void set_level(const Level& a_level) {
        m_level = a_level;
    }

    bool check(const Level& a_level) const {
        if(nullptr != m_file && a_level >= m_level) {
            return true;
        }
        return false;
    }

    void print_time(void) {
        struct timeval tv;
        struct tm *lt;

        gettimeofday(&tv, NULL); // 초 + 마이크로초 단위 시간 획득
        time_t sec = (time_t)tv.tv_sec; // 명시적 캐스팅
        lt = localtime(&sec);
        
        fprintf(m_file, 
                "[%02d%02d%02d-%02d%02d%02d.%06ld]",
                lt->tm_year % 100,
                lt->tm_mon + 1,
                lt->tm_mday,
                lt->tm_hour,
                lt->tm_min,
                lt->tm_sec,
                tv.tv_usec);
    }

    void print_hdr(const Level& a_level, const char* a_file, int a_line) {
        fprintf(m_file, "[%s][%s:%d] ", get_level_string(a_level), a_file, a_line);
    }

    void print_enter(void) {
        fprintf(m_file, "\n");
    }

private:
    Log() = default;
    Log(const Log&) = delete;
    Log& operator=(const Log&) = delete;
    Log(Log&&) = delete;
    Log& operator=(Log&&) = delete;

    const char* get_level_string(const Level& a_level) const {
        switch (a_level) {
            case Level::DEBUG:
                return "DEBUG";
            case Level::INFO:
                return "INFO";
            case Level::WARNING:
                return "WARNING";
            case Level::ERROR_LEVEL:
                return "ERROR";
        }
        return "UNKNOWN";
    }


    Level m_level = Level::INFO;
    FILE* m_file = nullptr;
};

#define LOG_INITIALIZE(a_file_path) omf::Log::instance().initialize(a_file_path)
#define LOG_FINALIZE() omf::Log::instance().finalize()
#define LOG_SET_LEVEL(a_level) omf::Log::instance().set_level(a_level)
#define LOG_PRINT(a_level, a_fmt, ...) {\
    if(omf::Log::instance().check(a_level)) {\
        omf::Log::instance().print_time(); \
        omf::Log::instance().print_hdr(a_level, __FILE__, __LINE__); \
        fprintf(omf::Log::instance().get_file(), a_fmt, ##__VA_ARGS__); \
        omf::Log::instance().print_enter(); \
    }\
}

} // namespace omf

#ifdef __cplusplus
}
#endif

#endif // LOG_HPP