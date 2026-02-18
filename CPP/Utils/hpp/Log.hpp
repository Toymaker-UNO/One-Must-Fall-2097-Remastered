#ifndef OPENOMF_UTILS_LOG_HPP
#define OPENOMF_UTILS_LOG_HPP

#include "../ExternalLibrary/hpp/Sdl2.hpp"
#include <cstdarg>
#include <cstdint>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <ctime>

namespace openomf {
namespace utils {

enum class LogLevel {
    DEBUG,
    INFO,
    WARN,
    ERR   /* C의 LOG_ERROR. Windows 매크로 ERROR 회피 */
};

namespace detail {

constexpr int LOG_MAX_TARGETS = 3;
constexpr int LOG_LEVEL_COUNT = 4;

struct LogTarget {
    FILE* fp = nullptr;
    LogLevel level = LogLevel::DEBUG;
    bool colors = false;
    bool close = false;
    SDL_mutex* lock = nullptr;
};

struct LogState {
    bool colors = false;
    LogLevel level = LogLevel::DEBUG;
    LogTarget targets[LOG_MAX_TARGETS];
    int target_count = 0;
};

inline char g_last_error[256];
inline LogState* g_state = nullptr;

inline const char* level_names[] = {"DEBUG", "INFO", "WARN", "ERR"};
inline const char* level_colors[] = {"\x1b[36m", "\x1b[32m", "\x1b[33m", "\x1b[31m"};

inline void close_targets()
{
    assert(g_state != nullptr);
    for (int i = 0; i < g_state->target_count; ++i) {
        LogTarget& t = g_state->targets[i];
        if (t.close && t.fp) {
            std::fclose(t.fp);
        }
        if (t.lock) {
            openomf::external_library::Sdl2::omf_sdl_destroy_mutex(t.lock);
        }
    }
    g_state->target_count = 0;
}

}  // namespace detail

/** C utils/log 대응. 외부 라이브러리(SDL mutex)는 ExternalLibrary/hpp(Sdl2) 통해서만 사용. */
class Log {
public:
    static void log_init()
    {
        assert(detail::g_state == nullptr);
        detail::g_state = new detail::LogState();
        detail::g_state->level = LogLevel::DEBUG;
        detail::g_state->colors = false;
    }

    static void log_close()
    {
        if (detail::g_state) {
            detail::close_targets();
            delete detail::g_state;
            detail::g_state = nullptr;
        }
    }

    static void log_set_level(LogLevel a_level)
    {
        assert(detail::g_state != nullptr);
        detail::g_state->level = a_level;
    }

    static void log_set_colors(bool a_toggle)
    {
        assert(detail::g_state != nullptr);
        detail::g_state->colors = a_toggle;
    }

    static void log_add_stderr(LogLevel a_level, bool a_colors)
    {
        log_add_fp(stderr, false, a_level, a_colors);
    }

    static void log_add_file(const char* a_filename, LogLevel a_level)
    {
        FILE* fp = std::fopen(a_filename, "w");
        if (fp) {
            log_add_fp(fp, true, a_level, false);
        }
    }

    static LogLevel log_level_text_to_enum(const char* a_level, LogLevel a_default_value)
    {
        for (int i = 0; i < detail::LOG_LEVEL_COUNT; ++i) {
            if (std::strncmp(detail::level_names[i], a_level, std::strlen(detail::level_names[i])) == 0) {
                return static_cast<LogLevel>(i);
            }
        }
        return a_default_value;
    }

    static bool is_log_level(const char* a_level)
    {
        for (int i = 0; i < detail::LOG_LEVEL_COUNT; ++i) {
            if (std::strcmp(detail::level_names[i], a_level) == 0) {
                return true;
            }
        }
        return false;
    }

    static void log_msg(LogLevel a_level, const char* a_fmt, ...)
    {
        assert(detail::g_state != nullptr);
        if (static_cast<int>(a_level) < static_cast<int>(detail::g_state->level)) {
            return;
        }

        char dt[16];
        format_timestamp(dt, sizeof(dt));
        const char* color = detail::level_colors[static_cast<int>(a_level)];
        const char* name = detail::level_names[static_cast<int>(a_level)];

        for (int i = 0; i < detail::g_state->target_count; ++i) {
            detail::LogTarget& target = detail::g_state->targets[i];
            if (static_cast<int>(a_level) < static_cast<int>(target.level)) {
                continue;
            }
            if (openomf::external_library::Sdl2::omf_sdl_lock_mutex(target.lock) != 0) {
                continue;
            }

            if (detail::g_state->colors && target.colors) {
                std::fprintf(target.fp, "%s %s%-5s\x1b[0m \x1b[0m ", dt, color, name);
            } else {
                std::fprintf(target.fp, "%s %-5s ", dt, name);
            }

            va_list args;
            va_start(args, a_fmt);
            std::vfprintf(target.fp, a_fmt, args);
            va_end(args);

            if (a_level == LogLevel::ERR) {
                va_start(args, a_fmt);
                std::vsnprintf(detail::g_last_error, sizeof(detail::g_last_error), a_fmt, args);
                va_end(args);
            }
            if (detail::g_state->colors && target.colors) {
                std::fprintf(target.fp, "\x1b[0m\n");
            } else {
                std::fprintf(target.fp, "\n");
            }
            std::fflush(target.fp);
            openomf::external_library::Sdl2::omf_sdl_unlock_mutex(target.lock);
        }
    }

    static const char* log_last_error()
    {
        return detail::g_last_error;
    }

private:
    static void log_add_fp(void* a_fp, bool a_close, LogLevel a_level, bool a_colors)
    {
        assert(detail::g_state != nullptr);
        assert(detail::g_state->target_count < detail::LOG_MAX_TARGETS - 1);
        detail::LogTarget& t = detail::g_state->targets[detail::g_state->target_count++];
        t.close = a_close;
        t.fp = static_cast<FILE*>(a_fp);
        t.level = a_level;
        t.colors = a_colors;
        t.lock = openomf::external_library::Sdl2::omf_sdl_create_mutex();
    }

    static void format_timestamp(char* a_buffer, size_t a_len)
    {
        std::time_t t = std::time(nullptr);
        std::tm* tm = std::localtime(&t);
        std::strftime(a_buffer, a_len, "%H:%M:%S", tm);
        a_buffer[a_len - 1] = '\0';
    }
};

}  // namespace utils
}  // namespace openomf

#endif
