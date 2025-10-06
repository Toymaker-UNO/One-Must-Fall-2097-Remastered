#include "log.hpp"
#include <iostream>

int main() {
    // 로거 초기화
    omf::Logger::instance().init();
    
    // 로그 레벨 설정
    omf::Logger::instance().set_level(omf::LogLevel::DEBUG);
    
    // 색상 활성화
    omf::Logger::instance().set_colors(true);
    
    // stderr에 로그 출력 (색상 포함)
    omf::Logger::instance().add_stderr(omf::LogLevel::DEBUG, true);
    
    // 파일에 로그 출력
    omf::Logger::instance().add_file("example.log", omf::LogLevel::INFO);
    
    // C 스타일 로깅 (printf 스타일)
    LOG_DEBUG("디버그 메시지: %d", 42);
    LOG_INFO("정보 메시지: %s", "Hello World");
    LOG_WARN("경고 메시지: %f", 3.14);
    LOG_ERROR("에러 메시지: %s", "Something went wrong");
    
    // C++ 스타일 로깅 (스트림 스타일)
    LOG_DEBUG_STREAM << "스트림 디버그: " << 42 << " " << 3.14;
    LOG_INFO_STREAM << "스트림 정보: " << std::string("Hello") << " " << "World";
    LOG_WARN_STREAM << "스트림 경고: " << std::hex << 0xDEADBEEF;
    LOG_ERROR_STREAM << "스트림 에러: " << std::boolalpha << false;
    
    // 직접 로거 사용
    omf::Logger::instance().log(omf::LogLevel::INFO, "직접 호출 메시지");
    omf::Logger::instance().log(omf::LogLevel::ERROR, std::string("문자열 메시지"));
    
    // 유틸리티 함수들
    std::cout << "DEBUG 레벨 문자열: " << omf::Logger::level_to_string(omf::LogLevel::DEBUG) << std::endl;
    std::cout << "INFO 유효한가? " << omf::Logger::is_valid_level("INFO") << std::endl;
    std::cout << "INVALID 유효한가? " << omf::Logger::is_valid_level("INVALID") << std::endl;
    
    auto level = omf::Logger::text_to_level("WARN", omf::LogLevel::DEBUG);
    std::cout << "WARN 텍스트를 레벨로 변환: " << omf::Logger::level_to_string(level) << std::endl;
    
    // 마지막 에러 메시지 확인
    std::cout << "마지막 에러: " << omf::Logger::instance().last_error() << std::endl;
    
    // 로거 정리
    omf::Logger::instance().close();
    
    return 0;
}
