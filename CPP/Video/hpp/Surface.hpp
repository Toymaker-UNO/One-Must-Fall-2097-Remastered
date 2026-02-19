#ifndef OPENOMF_VIDEO_SURFACE_HPP
#define OPENOMF_VIDEO_SURFACE_HPP

#include "hpp/Image.hpp"
#include "Formats/hpp/VgaImage.hpp"
#include "Formats/hpp/Palette.hpp"
#include "Utils/hpp/Allocator.hpp"
#include <cstring>
#include <cstdint>

namespace openomf {
namespace video {

/** C video/surface.h surface. 8-bit palette index surface. */
struct Surface {
    unsigned int guid = 0;
    int w = 0;
    int h = 0;
    int transparent = 0;
    unsigned char* data = nullptr;
    const formats::VgaPalette* palette = nullptr; // 배경/스프라이트 팔레트 (없으면 nullptr)
};

namespace detail {
inline static unsigned int s_next_guid = 0;
}

inline void surface_create(Surface* sur, int w, int h) {
    if (!sur || w <= 0 || h <= 0) return;
    sur->data = static_cast<unsigned char*>(omf_calloc(1, static_cast<size_t>(w) * h));
    sur->guid = detail::s_next_guid++;
    sur->w = w;
    sur->h = h;
    sur->transparent = 0;
}

inline void surface_create_from_vga(Surface* sur, const openomf::formats::VgaImage* src) {
    if (!sur || !src || !src->data) return;
    surface_create(sur, static_cast<int>(src->w), static_cast<int>(src->h));
    std::memcpy(sur->data, src->data, src->len);
    sur->transparent = -1;
}

inline void surface_create_from_data(Surface* sur, int w, int h, const unsigned char* src) {
    if (!sur) return;
    surface_create(sur, w, h);
    if (src) std::memcpy(sur->data, src, static_cast<size_t>(w) * h);
}

inline void surface_create_from(Surface* dst, const Surface* src) {
    if (!dst || !src) return;
    surface_create(dst, src->w, src->h);
    std::memcpy(dst->data, src->data, static_cast<size_t>(src->w) * src->h);
    dst->transparent = src->transparent;
}

inline void surface_free(Surface* sur) {
    if (!sur) return;
    openomf::utils::Allocator::free_real(sur->data);
    sur->data = nullptr;
    sur->w = 0;
    sur->h = 0;
}

inline void surface_clear(Surface* sur) {
    if (!sur || !sur->data) return;
    std::memset(sur->data, 0, static_cast<size_t>(sur->w) * sur->h);
    sur->guid = detail::s_next_guid++;
}

inline void surface_set_transparency(Surface* sur, int index) {
    if (sur) sur->transparent = index;
}

} // namespace video
} // namespace openomf

#endif // OPENOMF_VIDEO_SURFACE_HPP
