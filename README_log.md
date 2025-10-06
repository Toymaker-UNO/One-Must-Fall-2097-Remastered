# OMF Logging System (C++)

원본 C 코드의 `log.h`와 `log.c`를 C++로 포팅한 로깅 시스템입니다.

## 주요 특징

- **스레드 안전**: `std::mutex`를 사용한 스레드 안전한 로깅
- **다중 출력 타겟**: stderr, 파일 등 여러 출력 대상 지원
- **색상 지원**: 터미널에서 색상으로 로그 레벨 구분
- **C++ 스타일**: RAII, 스마트 포인터, 네임스페이스 활용
- **두 가지 로깅 스타일**: C 스타일(printf)과 C++ 스타일(스트림) 모두 지원

## 로그 레벨

- `DEBUG`: 디버그 정보
- `INFO`: 일반 정보
- `WARN`: 경고
- `ERROR`: 에러

## 사용법

### 1. 기본 설정

```cpp
#include "log.hpp"

// 로거 초기화
omf::Logger::instance().init();

// 로그 레벨 설정
omf::Logger::instance().set_level(omf::LogLevel::DEBUG);

// 색상 활성화
omf::Logger::instance().set_colors(true);

// 출력 타겟 추가
omf::Logger::instance().add_stderr(omf::LogLevel::DEBUG, true);  // stderr에 색상 포함
omf::Logger::instance().add_file("app.log", omf::LogLevel::INFO);  // 파일에 저장
```

### 2. C 스타일 로깅 (printf 스타일)

```cpp
LOG_DEBUG("디버그 메시지: %d", 42);
LOG_INFO("정보 메시지: %s", "Hello World");
LOG_WARN("경고 메시지: %f", 3.14);
LOG_ERROR("에러 메시지: %s", "Something went wrong");
```

### 3. C++ 스타일 로깅 (스트림 스타일)

```cpp
LOG_DEBUG_STREAM << "스트림 디버그: " << 42 << " " << 3.14;
LOG_INFO_STREAM << "스트림 정보: " << std::string("Hello") << " " << "World";
LOG_WARN_STREAM << "스트림 경고: " << std::hex << 0xDEADBEEF;
LOG_ERROR_STREAM << "스트림 에러: " << std::boolalpha << false;
```

### 4. 직접 로거 사용

```cpp
omf::Logger::instance().log(omf::LogLevel::INFO, "직접 호출 메시지");
omf::Logger::instance().log(omf::LogLevel::ERROR, std::string("문자열 메시지"));
```

### 5. 유틸리티 함수

```cpp
// 로그 레벨을 문자열로 변환
std::string level_str = omf::Logger::level_to_string(omf::LogLevel::DEBUG);

// 문자열이 유효한 로그 레벨인지 확인
bool is_valid = omf::Logger::is_valid_level("INFO");

// 문자열을 로그 레벨로 변환
auto level = omf::Logger::text_to_level("WARN", omf::LogLevel::DEBUG);

// 마지막 에러 메시지 가져오기
std::string last_error = omf::Logger::instance().last_error();
```

### 6. 정리

```cpp
// 프로그램 종료 시 로거 정리
omf::Logger::instance().close();
```

## 컴파일

```bash
make
```

또는 직접 컴파일:

```bash
g++ -std=c++11 -Wall -Wextra -O2 -o log_example log.cpp log_example.cpp
```

## 실행

```bash
./log_example
```

## 원본 C 코드와의 차이점

1. **네임스페이스**: `omf` 네임스페이스 사용
2. **클래스 기반**: 싱글톤 패턴으로 구현
3. **RAII**: 자동 리소스 관리
4. **스마트 포인터**: `std::unique_ptr` 사용
5. **STL 컨테이너**: `std::vector` 사용
6. **스트림 지원**: C++ 스타일 로깅 추가
7. **타입 안전성**: enum class 사용

## 파일 구조

- `log.hpp`: 헤더 파일
- `log.cpp`: 구현 파일
- `log_example.cpp`: 사용 예제
- `Makefile`: 빌드 설정
- `README_log.md`: 이 문서
