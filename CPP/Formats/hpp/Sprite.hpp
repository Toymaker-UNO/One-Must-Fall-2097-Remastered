#ifndef OPENOMF_FORMATS_SPRITE_HPP
#define OPENOMF_FORMATS_SPRITE_HPP

#include "Error.hpp"
#include "Reader.hpp"
#include "Writer.hpp"
#include "../Utils/hpp/Allocator.hpp"
#include <cstring>
#include <cstdint>

namespace openomf {
namespace formats {

/** C formats/sprite.h sd_sprite 대응. RLE 인코딩된 팔레트 스프라이트. */
struct Sprite {
    int16_t pos_x = 0;
    int16_t pos_y = 0;
    uint8_t index = 0;
    uint8_t missing = 0;  ///< 1이면 data는 다른 스프라이트 참조
    uint16_t width = 0;
    uint16_t height = 0;
    uint16_t len = 0;
    char* data = nullptr;
};

inline SdError sprite_create(Sprite* a_sprite) {
    if (!a_sprite) return SdError::INVALID_INPUT;
    std::memset(a_sprite, 0, sizeof(Sprite));
    return SdError::SUCCESS;
}

inline SdError sprite_copy(Sprite* a_dst, const Sprite* a_src) {
    if (!a_dst || !a_src) return SdError::INVALID_INPUT;
    std::memset(a_dst, 0, sizeof(Sprite));
    a_dst->pos_x = a_src->pos_x;
    a_dst->pos_y = a_src->pos_y;
    a_dst->index = a_src->index;
    a_dst->missing = a_src->missing;
    a_dst->width = a_src->width;
    a_dst->height = a_src->height;
    a_dst->len = a_src->len;
    if (a_src->data) {
        a_dst->data = static_cast<char*>(omf_calloc(a_src->len, 1));
        std::memcpy(a_dst->data, a_src->data, a_src->len);
    }
    return SdError::SUCCESS;
}

inline void sprite_free(Sprite* a_sprite) {
    if (!a_sprite) return;
    if (a_sprite->data && !a_sprite->missing) {
        omf_free(a_sprite->data);
        a_sprite->data = nullptr;
    }
}

inline SdError sprite_load(Reader* a_r, Sprite* a_sprite) {
    if (!a_r || !a_sprite) return SdError::INVALID_INPUT;
    a_sprite->len = a_r->read_uword();
    a_sprite->pos_x = a_r->read_word();
    a_sprite->pos_y = a_r->read_word();
    a_sprite->width = a_r->read_uword();
    a_sprite->height = a_r->read_uword();
    a_sprite->index = a_r->read_ubyte();
    a_sprite->missing = a_r->read_ubyte();
    if (a_sprite->missing == 0 && a_sprite->len != 0) {
        a_sprite->data = static_cast<char*>(omf_calloc(1, a_sprite->len));
        a_r->read_buf(a_sprite->data, a_sprite->len);
    } else {
        a_sprite->data = nullptr;
    }
    if (!a_r->ok()) return SdError::FILE_PARSE_ERROR;
    return SdError::SUCCESS;
}

inline SdError sprite_save(Writer* a_w, const Sprite* a_sprite) {
    if (!a_w || !a_sprite) return SdError::INVALID_INPUT;
    a_w->write_uword(a_sprite->len);
    a_w->write_word(a_sprite->pos_x);
    a_w->write_word(a_sprite->pos_y);
    a_w->write_uword(a_sprite->width);
    a_w->write_uword(a_sprite->height);
    a_w->write_ubyte(a_sprite->index);
    a_w->write_ubyte(a_sprite->missing);
    if (!a_sprite->missing && a_sprite->data)
        a_w->write_buf(a_sprite->data, a_sprite->len);
    return SdError::SUCCESS;
}

} // namespace formats
} // namespace openomf

#endif // OPENOMF_FORMATS_SPRITE_HPP
