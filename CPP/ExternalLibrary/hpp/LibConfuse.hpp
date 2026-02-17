#ifndef OPENOMF_EXTERNALLIBRARY_LIBCONFUSE_HPP
#define OPENOMF_EXTERNALLIBRARY_LIBCONFUSE_HPP

#include <confuse.h>
#include <cstdio>

namespace openomf {
namespace external_library {

/** libconfuse 라이브러리 정적 래퍼 (설정 파일 파싱). */
class LibConfuse {
public:
    /** 설정 컨텍스트 초기화. */
    static cfg_t* omf_cfg_init(cfg_opt_t* a_opts, cfg_flag_t a_flags) { return cfg_init(a_opts, a_flags); }

    /** 설정 파일 파싱. */
    static int omf_cfg_parse(cfg_t* a_cfg, const char* a_filename) { return cfg_parse(a_cfg, a_filename); }

    /** 설정 컨텍스트 해제. */
    static int omf_cfg_free(cfg_t* a_cfg) { return cfg_free(a_cfg); }

    /** 설정을 파일에 출력. */
    static int omf_cfg_print(cfg_t* a_cfg, FILE* a_fp) { return cfg_print(a_cfg, a_fp); }

    /** 정수 옵션 값 반환. */
    static long omf_cfg_getint(cfg_t* a_cfg, const char* a_name) { return cfg_getint(a_cfg, a_name); }

    /** 실수 옵션 값 반환. */
    static double omf_cfg_getfloat(cfg_t* a_cfg, const char* a_name) { return cfg_getfloat(a_cfg, a_name); }

    /** 불리언 옵션 값 반환. */
    static cfg_bool_t omf_cfg_getbool(cfg_t* a_cfg, const char* a_name) { return cfg_getbool(a_cfg, a_name); }

    /** 문자열 옵션 값 반환. */
    static char* omf_cfg_getstr(cfg_t* a_cfg, const char* a_name) { return cfg_getstr(a_cfg, a_name); }

    /** 정수 옵션 값 설정. */
    static int omf_cfg_setint(cfg_t* a_cfg, const char* a_name, long a_value) {
        return cfg_setint(a_cfg, a_name, a_value);
    }

    /** 실수 옵션 값 설정. */
    static int omf_cfg_setfloat(cfg_t* a_cfg, const char* a_name, double a_value) {
        return cfg_setfloat(a_cfg, a_name, a_value);
    }

    /** 불리언 옵션 값 설정. */
    static int omf_cfg_setbool(cfg_t* a_cfg, const char* a_name, cfg_bool_t a_value) {
        return cfg_setbool(a_cfg, a_name, a_value);
    }

    /** 문자열 옵션 값 설정. */
    static int omf_cfg_setstr(cfg_t* a_cfg, const char* a_name, const char* a_value) {
        return cfg_setstr(a_cfg, a_name, a_value);
    }
};

} // namespace external_library
} // namespace openomf

#endif // OPENOMF_EXTERNALLIBRARY_LIBCONFUSE_HPP
