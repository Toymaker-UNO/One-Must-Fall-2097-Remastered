#ifndef OPENOMF_EXTERNALLIBRARY_POSIX_HPP
#define OPENOMF_EXTERNALLIBRARY_POSIX_HPP

#include <cstdint>

#if !defined(_WIN32) && !defined(_WIN64)
#include <arpa/inet.h>
#include <dirent.h>
#include <fcntl.h>
#include <getopt.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

namespace openomf {
namespace external_library {

/** POSIX/기타 플랫폼 API 정적 래퍼 (C 소스에서 실제 사용 확인된 함수만). 비 Windows 빌드에서만 유효. */
class Posix {
public:
#if !defined(_WIN32) && !defined(_WIN64)
    /** 디렉터리 열기. */
    static DIR* omf_opendir(const char* a_name) { return ::opendir(a_name); }

    /** 디렉터리 엔트리 읽기. */
    static struct dirent* omf_readdir(DIR* a_dirp) { return ::readdir(a_dirp); }

    /** 디렉터리 닫기. */
    static int omf_closedir(DIR* a_dirp) { return ::closedir(a_dirp); }

    /** 디렉터리 생성. */
    static int omf_mkdir(const char* a_pathname, mode_t a_mode) { return ::mkdir(a_pathname, a_mode); }

    /** 호스트 바이트 오더를 네트워크 바이트 오더로 변환. */
    static std::uint32_t omf_htonl(std::uint32_t a_hostlong) { return ::htonl(a_hostlong); }

    /** 네트워크 바이트 오더를 호스트 바이트 오더로 변환. */
    static std::uint32_t omf_ntohl(std::uint32_t a_netlong) { return ::ntohl(a_netlong); }

    /** 파일 접근 가능 여부 (mode: F_OK 등, &lt;unistd.h&gt;). */
    static int omf_access(const char* a_pathname, int a_mode) { return ::access(a_pathname, a_mode); }
#endif
};

} // namespace external_library
} // namespace openomf

#endif // OPENOMF_EXTERNALLIBRARY_POSIX_HPP
