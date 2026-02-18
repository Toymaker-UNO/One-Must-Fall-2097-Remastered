#ifndef OPENOMF_VIDEO_IMAGE_HPP
#define OPENOMF_VIDEO_IMAGE_HPP

#include "Utils/hpp/Allocator.hpp"
#include <cstdint>
#include <cstring>

namespace openomf {
namespace video {

/** C video/image.h image_t. 8-bit palette index image. */
struct Image {
    unsigned int w = 0;
    unsigned int h = 0;
    unsigned char* data = nullptr;
};

inline int image_create(Image* img, int w, int h) {
    if (!img || w <= 0 || h <= 0) return -1;
    img->data = static_cast<unsigned char*>(omf_calloc(1, static_cast<size_t>(w) * h));
    img->w = static_cast<unsigned int>(w);
    img->h = static_cast<unsigned int>(h);
    return 0;
}

inline void image_free(Image* img) {
    if (!img) return;
    openomf::utils::Allocator::free_real(img->data);
    img->data = nullptr;
    img->w = 0;
    img->h = 0;
}

inline void image_clear(Image* img, uint8_t c) {
    if (!img || !img->data) return;
    std::memset(img->data, c, img->w * img->h);
}

} // namespace video
} // namespace openomf

#endif // OPENOMF_VIDEO_IMAGE_HPP
