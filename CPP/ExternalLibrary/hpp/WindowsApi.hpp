#ifndef OPENOMF_EXTERNALLIBRARY_WINDOWSAPI_HPP
#define OPENOMF_EXTERNALLIBRARY_WINDOWSAPI_HPP

#if defined(_WIN32) || defined(_WIN64)
#include <shlobj.h>
#include <shlwapi.h>
#include <windows.h>
#endif

namespace openomf {
namespace external_library {

/** Windows API 정적 래퍼 (경로/파일, List.md 명시 함수만). */
class WindowsApi {
public:
#if defined(_WIN32) || defined(_WIN64)
    /** 지정 경로의 파일 존재 여부 확인. */
    static BOOL omf_path_file_exists_a(LPCSTR a_psz_path) { return ::PathFileExistsA(a_psz_path); }

    /** 디렉터리 생성 (필요 시 상위 디렉터리 포함). A(ANSI) 버전. */
    static int omf_sh_create_directory_ex_a(HWND a_hwnd, LPCSTR a_psz_path,
                                            const SECURITY_ATTRIBUTES* a_psa) {
        return ::SHCreateDirectoryExA(a_hwnd, a_psz_path, a_psa);
    }
#endif
};

} // namespace external_library
} // namespace openomf

#endif // OPENOMF_EXTERNALLIBRARY_WINDOWSAPI_HPP
