#ifndef LOG_HPP
#define LOG_HPP

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <string>
#include <vector>
#include <mutex>
#include <memory>
#include <sstream>
#include <iomanip>

namespace omf {

enum class LogLevel : uint8_t {
    DEBUG = 0,
    INFO = 1,
    WARN = 2,
    ERROR = 3
};

class LogTarget {
public:
    LogTarget(FILE* fp, LogLevel level, bool colors, bool should_close);
    ~LogTarget();
    
    // 복사 생성자와 대입 연산자 비활성화
    LogTarget(const LogTarget&) = delete;
    LogTarget& operator=(const LogTarget&) = delete;
    
    // 이동 생성자와 이동 대입 연산자
    LogTarget(LogTarget&& other) noexcept;
    LogTarget& operator=(LogTarget&& other) noexcept;
    
    void write(LogLevel level, const std::string& message, bool use_colors);
    bool should_log(LogLevel level) const;
    
private:
    FILE* fp_;
    LogLevel level_;
    bool colors_;
    bool should_close_;
    std::unique_ptr<std::mutex> mutex_;
};

class Logger {
public:
    static constexpr size_t MAX_TARGETS = 3;
    
    static Logger& instance();
    
    // 초기화 및 정리
    void init();
    void close();
    
    // 설정
    void set_level(LogLevel level);
    void set_colors(bool enable);
    
    // 타겟 추가
    void add_stderr(LogLevel level, bool colors = true);
    void add_file(const std::string& filename, LogLevel level);
    
    // 로그 메시지 출력
    void log(LogLevel level, const char* fmt, ...);
    void log(LogLevel level, const std::string& message);
    
    // 유틸리티 함수
    static LogLevel text_to_level(const std::string& level_text, LogLevel default_value = LogLevel::DEBUG);
    static bool is_valid_level(const std::string& level_text);
    static std::string level_to_string(LogLevel level);
    
    // 마지막 에러 메시지
    const std::string& last_error() const;
    
    // 소멸자
    ~Logger();
    
private:
    Logger() = default;
    
    // 복사 생성자와 대입 연산자 비활성화
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    void format_timestamp(std::string& buffer);
    void write_to_targets(LogLevel level, const std::string& message);
    
    bool initialized_;
    bool colors_enabled_;
    LogLevel current_level_;
    std::vector<LogTarget> targets_;
    std::string last_error_;
    
    static const std::string level_names_[];
    static const std::string level_colors_[];
};

// 편의 매크로들 (C 스타일과 유사하게)
#define LOG_DEBUG(...) omf::Logger::instance().log(omf::LogLevel::DEBUG, __VA_ARGS__)
#define LOG_INFO(...) omf::Logger::instance().log(omf::LogLevel::INFO, __VA_ARGS__)
#define LOG_WARN(...) omf::Logger::instance().log(omf::LogLevel::WARN, __VA_ARGS__)
#define LOG_ERROR(...) omf::Logger::instance().log(omf::LogLevel::ERROR, __VA_ARGS__)

// 스트림 스타일 로깅을 위한 헬퍼 클래스
class LogStream {
public:
    LogStream(LogLevel level) : level_(level), stream_() {}
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        stream_ << value;
        return *this;
    }
    
    ~LogStream() {
        if (!stream_.str().empty()) {
            Logger::instance().log(level_, stream_.str());
        }
    }
    
private:
    LogLevel level_;
    std::ostringstream stream_;
};

// 스트림 스타일 매크로들
#define LOG_DEBUG_STREAM omf::LogStream(omf::LogLevel::DEBUG)
#define LOG_INFO_STREAM omf::LogStream(omf::LogLevel::INFO)
#define LOG_WARN_STREAM omf::LogStream(omf::LogLevel::WARN)
#define LOG_ERROR_STREAM omf::LogStream(omf::LogLevel::ERROR)

} // namespace omf

#endif // LOG_HPP
