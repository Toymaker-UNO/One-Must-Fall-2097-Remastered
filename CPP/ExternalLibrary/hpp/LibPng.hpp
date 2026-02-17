#ifndef OPENOMF_EXTERNALLIBRARY_LIBPNG_HPP
#define OPENOMF_EXTERNALLIBRARY_LIBPNG_HPP

#include <png.h>
#include <cstddef>
#include <cstdio>

namespace openomf {
namespace external_library {

/** libpng 라이브러리 정적 래퍼 (PNG 읽기/쓰기). */
class LibPng {
public:
    /** 시그니처가 PNG 시그니처인지 검사 (png_sig_cmp 래핑). */
    static int omf_png_check_sig(png_const_bytep a_sig, int a_num) {
        return png_sig_cmp(a_sig, 0, static_cast<std::size_t>(a_num)) == 0;
    }

    /** 읽기용 png_struct 생성. */
    static png_structp omf_png_create_read_struct(png_const_charp a_user_png_ver, png_voidp a_error_ptr,
                                                  png_error_ptr a_error_fn, png_error_ptr a_warn_fn) {
        return png_create_read_struct(a_user_png_ver, a_error_ptr, a_error_fn, a_warn_fn);
    }

    /** info 구조체 생성. */
    static png_infop omf_png_create_info_struct(png_const_structrp a_png_ptr) {
        return png_create_info_struct(a_png_ptr);
    }

    /** 읽기용 png_struct 및 info 해제. */
    static void omf_png_destroy_read_struct(png_structpp a_png_ptr_ptr, png_infopp a_info_ptr_ptr,
                                            png_infopp a_end_info_ptr_ptr) {
        png_destroy_read_struct(a_png_ptr_ptr, a_info_ptr_ptr, a_end_info_ptr_ptr);
    }

    /** png_struct에 파일 스트림 연결. */
    static void omf_png_init_io(png_structrp a_png_ptr, FILE* a_fp) { png_init_io(a_png_ptr, a_fp); }

    /** 이미 읽은 시그니처 바이트 수 설정. */
    static void omf_png_set_sig_bytes(png_structrp a_png_ptr, int a_num_bytes) {
        png_set_sig_bytes(a_png_ptr, a_num_bytes);
    }

    /** PNG 헤더 정보 읽기. */
    static void omf_png_read_info(png_structrp a_png_ptr, png_inforp a_info_ptr) {
        png_read_info(a_png_ptr, a_info_ptr);
    }

    /** 이미지 너비 반환. */
    static png_uint_32 omf_png_get_image_width(png_const_structrp a_png_ptr, png_const_inforp a_info_ptr) {
        return png_get_image_width(a_png_ptr, a_info_ptr);
    }

    /** 이미지 높이 반환. */
    static png_uint_32 omf_png_get_image_height(png_const_structrp a_png_ptr, png_const_inforp a_info_ptr) {
        return png_get_image_height(a_png_ptr, a_info_ptr);
    }

    /** 컬러 타입 반환. */
    static png_byte omf_png_get_color_type(png_const_structrp a_png_ptr, png_const_inforp a_info_ptr) {
        return png_get_color_type(a_png_ptr, a_info_ptr);
    }

    /** 비트 깊이 반환. */
    static png_byte omf_png_get_bit_depth(png_const_structrp a_png_ptr, png_const_inforp a_info_ptr) {
        return png_get_bit_depth(a_png_ptr, a_info_ptr);
    }

    /** 한 행의 바이트 수 반환. */
    static std::size_t omf_png_get_rowbytes(png_const_structrp a_png_ptr, png_const_inforp a_info_ptr) {
        return png_get_rowbytes(a_png_ptr, a_info_ptr);
    }

    /** libpng 할당자로 메모리 할당. */
    static png_voidp omf_png_malloc(png_const_structrp a_png_ptr, png_alloc_size_t a_size) {
        return png_malloc(a_png_ptr, a_size);
    }

    /** 전체 이미지 픽셀 읽기. */
    static void omf_png_read_image(png_structrp a_png_ptr, png_bytepp a_image) {
        png_read_image(a_png_ptr, a_image);
    }

    /** libpng 할당 메모리 해제. */
    static void omf_png_free(png_const_structrp a_png_ptr, png_voidp a_ptr) { png_free(a_png_ptr, a_ptr); }

    /** 쓰기용 png_struct 생성. */
    static png_structp omf_png_create_write_struct(png_const_charp a_user_png_ver, png_voidp a_error_ptr,
                                                    png_error_ptr a_error_fn, png_error_ptr a_warn_fn) {
        return png_create_write_struct(a_user_png_ver, a_error_ptr, a_error_fn, a_warn_fn);
    }

    /** 쓰기용 png_struct 해제. */
    static void omf_png_destroy_write_struct(png_structpp a_png_ptr_ptr, png_infopp a_info_ptr_ptr) {
        png_destroy_write_struct(a_png_ptr_ptr, a_info_ptr_ptr);
    }

    /** IHDR 청크 설정. */
    static void omf_png_set_IHDR(png_const_structrp a_png_ptr, png_inforp a_info_ptr, png_uint_32 a_width,
                                 png_uint_32 a_height, int a_bit_depth, int a_color_type, int a_interlace_method,
                                 int a_compression_method, int a_filter_method) {
        png_set_IHDR(a_png_ptr, a_info_ptr, a_width, a_height, a_bit_depth, a_color_type, a_interlace_method,
                     a_compression_method, a_filter_method);
    }

    /** 쓰기 전 정보 기록. */
    static void omf_png_write_info(png_structrp a_png_ptr, png_const_inforp a_info_ptr) {
        png_write_info(a_png_ptr, a_info_ptr);
    }

    /** 한 행 쓰기. */
    static void omf_png_write_row(png_structrp a_png_ptr, png_const_bytep a_row) {
        png_write_row(a_png_ptr, a_row);
    }

    /** 쓰기 완료. */
    static void omf_png_write_end(png_structrp a_png_ptr, png_inforp a_info_ptr) {
        png_write_end(a_png_ptr, a_info_ptr);
    }

    /** png_image로 파일에 이미지 쓰기. */
    static int omf_png_image_write_to_file(png_imagep a_image, const char* a_file, int a_convert_to_8bit,
                                           const void* a_buffer, png_int_32 a_row_stride, const void* a_colormap) {
        return png_image_write_to_file(a_image, a_file, a_convert_to_8bit, a_buffer, a_row_stride, a_colormap);
    }
};

} // namespace external_library
} // namespace openomf

#endif // OPENOMF_EXTERNALLIBRARY_LIBPNG_HPP
