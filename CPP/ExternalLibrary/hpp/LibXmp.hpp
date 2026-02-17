#ifndef OPENOMF_EXTERNALLIBRARY_LIBXMP_HPP
#define OPENOMF_EXTERNALLIBRARY_LIBXMP_HPP

#include <xmp.h>

namespace openomf {
namespace external_library {

/** libxmp 라이브러리 정적 래퍼 (모듈 음악 재생). */
class LibXmp {
public:
    /** 플레이어 컨텍스트 생성. */
    static xmp_context omf_xmp_create_context() { return xmp_create_context(); }

    /** 플레이어 컨텍스트 해제. */
    static void omf_xmp_free_context(xmp_context a_ctx) { xmp_free_context(a_ctx); }

    /** 파일에서 모듈 로드. */
    static int omf_xmp_load_module(xmp_context a_ctx, const char* a_path) {
        return xmp_load_module(a_ctx, a_path);
    }

    /** 로드된 모듈 해제. */
    static void omf_xmp_release_module(xmp_context a_ctx) { xmp_release_module(a_ctx); }

    /** 재생 시작. */
    static int omf_xmp_start_player(xmp_context a_ctx, int a_sample_rate, int a_format) {
        return xmp_start_player(a_ctx, a_sample_rate, a_format);
    }

    /** 재생 중지. */
    static void omf_xmp_end_player(xmp_context a_ctx) { xmp_end_player(a_ctx); }

    /** 버퍼에 PCM 렌더링. */
    static int omf_xmp_play_buffer(xmp_context a_ctx, void* a_buffer, int a_size, int a_loop) {
        return xmp_play_buffer(a_ctx, a_buffer, a_size, a_loop);
    }

    /** 모듈 정보 조회. */
    static void omf_xmp_get_module_info(xmp_context a_ctx, struct xmp_module_info* a_info) {
        xmp_get_module_info(a_ctx, a_info);
    }

    /** 플레이어 파라미터 설정. */
    static int omf_xmp_set_player(xmp_context a_ctx, int a_param, int a_value) {
        return xmp_set_player(a_ctx, a_param, a_value);
    }
};

} // namespace external_library
} // namespace openomf

#endif // OPENOMF_EXTERNALLIBRARY_LIBXMP_HPP
