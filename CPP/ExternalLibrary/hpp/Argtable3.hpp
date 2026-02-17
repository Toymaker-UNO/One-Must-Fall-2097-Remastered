#ifndef OPENOMF_EXTERNALLIBRARY_ARGTABLE3_HPP
#define OPENOMF_EXTERNALLIBRARY_ARGTABLE3_HPP

#include <argtable3.h>
#include <cstddef>
#include <cstdio>

namespace openomf {
namespace external_library {

/** argtable3 라이브러리 정적 래퍼 (CLI 인자 파싱, vendored). */
class Argtable3 {
public:
    /** 리터럴(플래그) 옵션 0개 이상 생성. */
    static struct arg_lit* omf_arg_lit0(const char* a_shortopts, const char* a_longopts, const char* a_glossary) {
        return arg_lit0(a_shortopts, a_longopts, a_glossary);
    }

    /** 문자열 옵션 0개 이상 생성. */
    static struct arg_str* omf_arg_str0(const char* a_shortopts, const char* a_longopts, const char* a_datatype,
                                        const char* a_glossary) {
        return arg_str0(a_shortopts, a_longopts, a_datatype, a_glossary);
    }

    /** 정수 옵션 0개 이상 생성. */
    static struct arg_int* omf_arg_int0(const char* a_shortopts, const char* a_longopts, const char* a_datatype,
                                        const char* a_glossary) {
        return arg_int0(a_shortopts, a_longopts, a_datatype, a_glossary);
    }

    /** 파일 옵션 0개 이상 생성. */
    static struct arg_file* omf_arg_file0(const char* a_shortopts, const char* a_longopts, const char* a_datatype,
                                          const char* a_glossary) {
        return arg_file0(a_shortopts, a_longopts, a_datatype, a_glossary);
    }

    /** 파서 종료(에러 수집) 인자 생성. */
    static struct arg_end* omf_arg_end(int a_maxcount) { return arg_end(a_maxcount); }

    /** 인자 테이블에 NULL이 없는지 검사. */
    static int omf_arg_nullcheck(void** a_argtable) { return arg_nullcheck(a_argtable); }

    /** 명령줄 인자 파싱. */
    static int omf_arg_parse(int a_argc, char** a_argv, void** a_argtable) {
        return arg_parse(a_argc, a_argv, a_argtable);
    }

    /** 사용법 문법 출력. */
    static void omf_arg_print_syntax(FILE* a_fp, void** a_argtable, const char* a_suffix) {
        arg_print_syntax(a_fp, a_argtable, a_suffix);
    }

    /** 옵션 설명(글로서리) 출력. */
    static void omf_arg_print_glossary(FILE* a_fp, void** a_argtable, const char* a_format) {
        arg_print_glossary(a_fp, a_argtable, a_format);
    }

    /** 파싱 에러 메시지 출력. */
    static void omf_arg_print_errors(FILE* a_fp, struct arg_end* a_end, const char* a_progname) {
        arg_print_errors(a_fp, a_end, a_progname);
    }

    /** 인자 테이블 메모리 해제. */
    static void omf_arg_freetable(void** a_argtable, std::size_t a_n) { arg_freetable(a_argtable, a_n); }
};

} // namespace external_library
} // namespace openomf

#endif // OPENOMF_EXTERNALLIBRARY_ARGTABLE3_HPP
