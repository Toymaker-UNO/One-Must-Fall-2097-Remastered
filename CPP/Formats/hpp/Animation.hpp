#ifndef OPENOMF_FORMATS_ANIMATION_HPP
#define OPENOMF_FORMATS_ANIMATION_HPP

#include "Error.hpp"
#include "ColCoord.hpp"
#include "Sprite.hpp"
#include "Reader.hpp"
#include "Writer.hpp"
#include "../Utils/hpp/Allocator.hpp"
#include <cstring>
#include <cstdint>

namespace openomf {
namespace formats {

constexpr int SD_ANIMATION_STRING_MAX = 1024;
constexpr int SD_EXTRA_STRING_MAX = 512;
constexpr int SD_SPRITE_COUNT_MAX = 255;
constexpr int SD_COLCOORD_COUNT_MAX = 256;
constexpr int SD_EXTRASTR_COUNT_MAX = 10;

/** C sd_animation 대응. */
struct Animation {
    int16_t start_x = 0;
    int16_t start_y = 0;
    int32_t null = 0;
    uint16_t coord_count = 0;
    uint8_t sprite_count = 0;
    uint8_t extra_string_count = 0;
    Coord coord_table[SD_COLCOORD_COUNT_MAX] = {};
    Sprite* sprites[SD_SPRITE_COUNT_MAX] = {};
    char anim_string[SD_ANIMATION_STRING_MAX] = {};
    char extra_strings[SD_EXTRASTR_COUNT_MAX][SD_EXTRA_STRING_MAX] = {};
};

inline SdError animation_create(Animation* a_ani) {
    if (!a_ani) return SdError::INVALID_INPUT;
    std::memset(a_ani, 0, sizeof(Animation));
    return SdError::SUCCESS;
}

inline SdError animation_copy(Animation* a_dst, const Animation* a_src) {
    if (!a_dst || !a_src) return SdError::INVALID_INPUT;
    std::memset(a_dst, 0, sizeof(Animation));
    a_dst->start_x = a_src->start_x;
    a_dst->start_y = a_src->start_y;
    a_dst->null = a_src->null;
    a_dst->coord_count = a_src->coord_count;
    a_dst->sprite_count = a_src->sprite_count;
    a_dst->extra_string_count = a_src->extra_string_count;
    std::memcpy(a_dst->coord_table, a_src->coord_table, sizeof(a_src->coord_table));
    std::memcpy(a_dst->anim_string, a_src->anim_string, sizeof(a_src->anim_string));
    std::memcpy(a_dst->extra_strings, a_src->extra_strings, sizeof(a_src->extra_strings));
    for (int i = 0; i < SD_SPRITE_COUNT_MAX; i++) {
        if (a_src->sprites[i]) {
            a_dst->sprites[i] = static_cast<Sprite*>(omf_calloc(1, sizeof(Sprite)));
            sprite_copy(a_dst->sprites[i], a_src->sprites[i]);
        }
    }
    return SdError::SUCCESS;
}

inline void animation_free(Animation* a_anim) {
    if (!a_anim) return;
    for (int i = 0; i < SD_SPRITE_COUNT_MAX; i++) {
        if (a_anim->sprites[i]) {
            sprite_free(a_anim->sprites[i]);
            openomf::utils::Allocator::free_real(a_anim->sprites[i]);
            a_anim->sprites[i] = nullptr;
        }
    }
}

inline int animation_get_coord_count(const Animation* a_ani) { return a_ani->coord_count; }
inline SdError animation_set_coord(Animation* a_ani, int a_num, const Coord& a_coord) {
    if (a_num < 0 || a_num >= a_ani->coord_count) return SdError::INVALID_INPUT;
    a_ani->coord_table[a_num] = a_coord;
    return SdError::SUCCESS;
}
inline SdError animation_push_coord(Animation* a_ani, const Coord& a_coord) {
    if (a_ani->coord_count >= SD_COLCOORD_COUNT_MAX) return SdError::INVALID_INPUT;
    a_ani->coord_table[a_ani->coord_count++] = a_coord;
    return SdError::SUCCESS;
}
inline SdError animation_pop_coord(Animation* a_ani) {
    if (a_ani->coord_count <= 0) return SdError::INVALID_INPUT;
    a_ani->coord_count--;
    return SdError::SUCCESS;
}
inline Coord* animation_get_coord(Animation* a_ani, int a_num) {
    if (a_num < 0 || a_num >= SD_COLCOORD_COUNT_MAX) return nullptr;
    return &a_ani->coord_table[a_num];
}

inline SdError animation_set_anim_string(Animation* a_ani, const char* a_str) {
    size_t len = std::strlen(a_str);
    if (len >= SD_ANIMATION_STRING_MAX) return SdError::INVALID_INPUT;
    std::strncpy(a_ani->anim_string, a_str, SD_ANIMATION_STRING_MAX - 1);
    a_ani->anim_string[SD_ANIMATION_STRING_MAX - 1] = '\0';
    return SdError::SUCCESS;
}
inline int animation_get_extra_string_count(const Animation* a_anim) { return a_anim->extra_string_count; }
inline SdError animation_set_extra_string(Animation* a_ani, int a_num, const char* a_str) {
    if (a_num < 0 || a_num >= a_ani->extra_string_count) return SdError::INVALID_INPUT;
    if (std::strlen(a_str) >= SD_EXTRA_STRING_MAX) return SdError::INVALID_INPUT;
    std::strncpy(a_ani->extra_strings[a_num], a_str, SD_EXTRA_STRING_MAX - 1);
    a_ani->extra_strings[a_num][SD_EXTRA_STRING_MAX - 1] = '\0';
    return SdError::SUCCESS;
}
inline char* animation_get_extra_string(Animation* a_anim, int a_num) {
    if (a_num < 0 || a_num >= a_anim->extra_string_count) return nullptr;
    return a_anim->extra_strings[a_num];
}
inline int animation_get_sprite_count(const Animation* a_anim) { return a_anim->sprite_count; }
inline Sprite* animation_get_sprite(Animation* a_anim, int a_num) {
    if (a_num < 0 || a_num >= SD_SPRITE_COUNT_MAX) return nullptr;
    return a_anim->sprites[a_num];
}

inline SdError animation_load(Reader* a_r, Animation* a_ani) {
    if (!a_r || !a_ani) return SdError::INVALID_INPUT;
    a_ani->start_x = a_r->read_word();
    a_ani->start_y = a_r->read_word();
    a_ani->null = static_cast<int32_t>(a_r->read_udword());
    a_ani->coord_count = a_r->read_uword();
    a_ani->sprite_count = a_r->read_ubyte();
    if (a_ani->coord_count > SD_COLCOORD_COUNT_MAX) return SdError::FILE_PARSE_ERROR;
    for (int i = 0; i < a_ani->coord_count; i++) {
        uint32_t tmp = a_r->read_udword();
        uint16_t a = tmp & 0xffffu;
        uint16_t b = (tmp >> 16) & 0xffffu;
        a_ani->coord_table[i].x = static_cast<int16_t>(((a & 0x3ffu) ^ 0x200u) - 0x200);
        a_ani->coord_table[i].null = static_cast<uint8_t>(a >> 10);
        a_ani->coord_table[i].y = static_cast<int16_t>(((b & 0x3ffu) ^ 0x200u) - 0x200);
        a_ani->coord_table[i].frame_id = static_cast<uint8_t>(b >> 10);
    }
    uint16_t size = a_r->read_uword();
    if (size >= SD_ANIMATION_STRING_MAX) return SdError::FILE_PARSE_ERROR;
    a_r->read_buf(a_ani->anim_string, size + 1);
    if (a_ani->anim_string[size] != 0) return SdError::FILE_PARSE_ERROR;
    a_ani->extra_string_count = a_r->read_ubyte();
    if (a_ani->extra_string_count > SD_EXTRASTR_COUNT_MAX) return SdError::FILE_PARSE_ERROR;
    for (int i = 0; i < a_ani->extra_string_count; i++) {
        size = a_r->read_uword();
        if (size >= SD_EXTRA_STRING_MAX) return SdError::FILE_PARSE_ERROR;
        a_r->read_buf(a_ani->extra_strings[i], size + 1);
        if (a_ani->extra_strings[i][size] != 0) return SdError::FILE_PARSE_ERROR;
    }
    for (int i = 0; i < a_ani->sprite_count; i++) {
        a_ani->sprites[i] = static_cast<Sprite*>(omf_calloc(1, sizeof(Sprite)));
        if (sprite_create(a_ani->sprites[i]) != SdError::SUCCESS) return SdError::OUT_OF_MEMORY;
        if (sprite_load(a_r, a_ani->sprites[i]) != SdError::SUCCESS) return SdError::FILE_PARSE_ERROR;
    }
    return SdError::SUCCESS;
}

inline SdError animation_save(Writer* a_w, const Animation* a_ani) {
    if (!a_ani || !a_w) return SdError::INVALID_INPUT;
    a_w->write_word(a_ani->start_x);
    a_w->write_word(a_ani->start_y);
    a_w->write_udword(static_cast<uint32_t>(a_ani->null));
    a_w->write_uword(a_ani->coord_count);
    a_w->write_ubyte(a_ani->sprite_count);
    for (int i = 0; i < a_ani->coord_count; i++) {
        uint32_t tmp = (a_ani->coord_table[i].frame_id & 0x3fu) << 10;
        tmp = (tmp | (a_ani->coord_table[i].y & 0x3ffu)) << 6;
        tmp = (tmp | (a_ani->coord_table[i].null & 0x3fu)) << 10;
        tmp |= (a_ani->coord_table[i].x & 0x3ffu);
        a_w->write_udword(tmp);
    }
    uint16_t size = static_cast<uint16_t>(std::strlen(a_ani->anim_string));
    a_w->write_uword(size);
    a_w->write_buf(a_ani->anim_string, size);
    a_w->write_ubyte(0);
    a_w->write_ubyte(a_ani->extra_string_count);
    for (int i = 0; i < a_ani->extra_string_count; i++) {
        size = static_cast<uint16_t>(std::strlen(a_ani->extra_strings[i]));
        a_w->write_uword(size);
        a_w->write_buf(a_ani->extra_strings[i], size);
        a_w->write_ubyte(0);
    }
    for (int i = 0; i < a_ani->sprite_count; i++) {
        if (sprite_save(a_w, a_ani->sprites[i]) != SdError::SUCCESS) return SdError::FILE_WRITE_ERROR;
    }
    return SdError::SUCCESS;
}

} // namespace formats
} // namespace openomf

#endif // OPENOMF_FORMATS_ANIMATION_HPP
