#ifndef OPENOMF_FORMATS_VGAIMAGE_HPP
#define OPENOMF_FORMATS_VGAIMAGE_HPP

#include "Error.hpp"
#include "../Utils/hpp/Allocator.hpp"
#include <cstring>
#include <cstdint>

namespace openomf {
namespace formats {

/** C formats/vga_image.h sd_vga_image 대응. 팔레트 인덱스 이미지. */
struct VgaImage {
    unsigned int w = 0;
    unsigned int h = 0;
    unsigned int len = 0;
    char* data = nullptr;
};

inline SdError vga_image_create(VgaImage* a_img, unsigned int a_w, unsigned int a_h) {
    if (!a_img) return SdError::INVALID_INPUT;
    a_img->w = a_w;
    a_img->h = a_h;
    a_img->len = a_w * a_h;
    a_img->data = static_cast<char*>(omf_calloc(1, a_img->len));
    return SdError::SUCCESS;
}

inline SdError vga_image_copy(VgaImage* a_dst, const VgaImage* a_src) {
    if (!a_dst || !a_src) return SdError::INVALID_INPUT;
    a_dst->w = a_src->w;
    a_dst->h = a_src->h;
    a_dst->len = a_src->len;
    a_dst->data = static_cast<char*>(omf_calloc(a_src->len, 1));
    std::memcpy(a_dst->data, a_src->data, a_src->len);
    return SdError::SUCCESS;
}

inline void vga_image_free(VgaImage* a_img) {
    if (!a_img) return;
    omf_free(a_img->data);
    a_img->data = nullptr;
    a_img->len = 0;
}

} // namespace formats
} // namespace openomf

#endif // OPENOMF_FORMATS_VGAIMAGE_HPP
