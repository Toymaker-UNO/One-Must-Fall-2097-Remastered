#ifndef OPENOMF_UTILS_IO_HPP
#define OPENOMF_UTILS_IO_HPP

#include "Log.hpp"
#include "MiscMath.hpp"
#include <cstdio>
#include <sys/stat.h>

#if defined(_WIN32) || defined(WIN32)
#include "../ExternalLibrary/hpp/WindowsApi.hpp"
#else
#include "../ExternalLibrary/hpp/Posix.hpp"
#include <unistd.h>  // F_OK
#endif

namespace openomf {
namespace utils {

/** C utils/io 대응. file_exists는 Windows에서 WindowsApi, 그 외 Posix(access) 사용. */
class Io {
public:
    static FILE* file_open(const char* a_file_name, const char* a_mode)
    {
        FILE* handle = std::fopen(a_file_name, a_mode);
        /* 실패 시 로그는 호출자에서 필요할 때만 남김 (존재 여부 확인 등에서는 로그 안 함) */
        return handle;
    }

    static long file_size(FILE* a_handle)
    {
        struct stat info;
        if (fstat(fileno(a_handle), &info) != 0) {
            return 0;
        }
        return static_cast<long>(info.st_size);
    }

    static bool file_read(FILE* a_handle, char* a_buffer, long a_size)
    {
        constexpr long READ_BLOCK_SIZE = 1024;
        size_t ptr = 0;
        while (true) {
            if (std::feof(a_handle)) {
                break;
            }
            if (std::ferror(a_handle)) {
                Log::log_msg(LogLevel::ERR, "Error while reading file");
                return false;
            }
            size_t read_size = static_cast<size_t>(MiscMath::min2(
                static_cast<int>(a_size - static_cast<long>(ptr)),
                static_cast<int>(READ_BLOCK_SIZE)));
            if (read_size == 0) {
                break;
            }
            size_t n = std::fread(a_buffer + ptr, 1, read_size, a_handle);
            ptr += n;
            if (n < read_size) {
                break;
            }
        }
        return true;
    }

    static void file_close(FILE* a_handle)
    {
        if (a_handle) {
            std::fclose(a_handle);
        }
    }

    static bool file_exists(const char* a_file_name)
    {
#if defined(_WIN32) || defined(WIN32)
        return openomf::external_library::WindowsApi::omf_path_file_exists_a(a_file_name) == TRUE;
#else
        return openomf::external_library::Posix::omf_access(a_file_name, F_OK) == 0;
#endif
    }
};

}  // namespace utils
}  // namespace openomf

#endif
