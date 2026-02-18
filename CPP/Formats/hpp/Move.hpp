#ifndef OPENOMF_FORMATS_MOVE_HPP
#define OPENOMF_FORMATS_MOVE_HPP

#include "Error.hpp"
#include "Animation.hpp"
#include "Reader.hpp"
#include "Writer.hpp"
#include "../Utils/hpp/Allocator.hpp"
#include <cstring>
#include <cstdint>

namespace openomf {
namespace formats {

constexpr int SD_MOVE_STRING_MAX = 21;
constexpr int SD_MOVE_FOOTER_STRING_MAX = 512;

/** C sd_move 대응. */
struct Move {
    Animation* animation = nullptr;
    uint16_t ai_opts = 0;
    uint16_t pos_constraint = 0;
    uint8_t unknown_4 = 0, unknown_5 = 0, unknown_6 = 0, unknown_7 = 0;
    uint8_t unknown_8 = 0, unknown_9 = 0, unknown_10 = 0, unknown_11 = 0;
    uint8_t next_anim_id = 0, category = 0, block_damage = 0, block_stun = 0;
    uint8_t successor_id = 0, damage_amount = 0, throw_duration = 0;
    uint8_t extra_string_selector = 0, points = 0;
    char move_string[SD_MOVE_STRING_MAX] = {};
    char footer_string[SD_MOVE_FOOTER_STRING_MAX] = {};
};

inline SdError move_create(Move* a_move) {
    if (!a_move) return SdError::INVALID_INPUT;
    std::memset(a_move, 0, sizeof(Move));
    return SdError::SUCCESS;
}

inline SdError move_copy(Move* a_dst, const Move* a_src) {
    if (!a_dst || !a_src) return SdError::INVALID_INPUT;
    std::memset(a_dst, 0, sizeof(Move));
    if (a_src->animation) {
        a_dst->animation = static_cast<Animation*>(omf_calloc(1, sizeof(Animation)));
        animation_copy(a_dst->animation, a_src->animation);
    }
    std::strncpy(a_dst->move_string, a_src->move_string, SD_MOVE_STRING_MAX - 1);
    std::strncpy(a_dst->footer_string, a_src->footer_string, SD_MOVE_FOOTER_STRING_MAX - 1);
    a_dst->ai_opts = a_src->ai_opts;
    a_dst->pos_constraint = a_src->pos_constraint;
    a_dst->unknown_4 = a_src->unknown_4;
    a_dst->unknown_5 = a_src->unknown_5;
    a_dst->unknown_6 = a_src->unknown_6;
    a_dst->unknown_7 = a_src->unknown_7;
    a_dst->unknown_8 = a_src->unknown_8;
    a_dst->unknown_9 = a_src->unknown_9;
    a_dst->unknown_10 = a_src->unknown_10;
    a_dst->unknown_11 = a_src->unknown_11;
    a_dst->next_anim_id = a_src->next_anim_id;
    a_dst->category = a_src->category;
    a_dst->block_damage = a_src->block_damage;
    a_dst->block_stun = a_src->block_stun;
    a_dst->successor_id = a_src->successor_id;
    a_dst->damage_amount = a_src->damage_amount;
    a_dst->throw_duration = a_src->throw_duration;
    a_dst->extra_string_selector = a_src->extra_string_selector;
    a_dst->points = a_src->points;
    return SdError::SUCCESS;
}

inline void move_free(Move* a_move) {
    if (!a_move) return;
    if (a_move->animation) {
        animation_free(a_move->animation);
        openomf::utils::Allocator::free_real(a_move->animation);
        a_move->animation = nullptr;
    }
}

inline SdError move_load(Reader* a_r, Move* a_move) {
    if (!a_r || !a_move) return SdError::INVALID_INPUT;
    a_move->animation = static_cast<Animation*>(omf_calloc(1, sizeof(Animation)));
    if (animation_create(a_move->animation) != SdError::SUCCESS) return SdError::OUT_OF_MEMORY;
    if (animation_load(a_r, a_move->animation) != SdError::SUCCESS) return SdError::FILE_PARSE_ERROR;
    a_move->ai_opts = a_r->read_uword();
    a_move->pos_constraint = a_r->read_uword();
    a_move->unknown_4 = a_r->read_ubyte();
    a_move->unknown_5 = a_r->read_ubyte();
    a_move->unknown_6 = a_r->read_ubyte();
    a_move->unknown_7 = a_r->read_ubyte();
    a_move->unknown_8 = a_r->read_ubyte();
    a_move->unknown_9 = a_r->read_ubyte();
    a_move->unknown_10 = a_r->read_ubyte();
    a_move->unknown_11 = a_r->read_ubyte();
    a_move->next_anim_id = a_r->read_ubyte();
    a_move->category = a_r->read_ubyte();
    a_move->block_damage = a_r->read_ubyte();
    a_move->block_stun = a_r->read_ubyte();
    a_move->successor_id = a_r->read_ubyte();
    a_move->damage_amount = a_r->read_ubyte();
    a_move->throw_duration = a_r->read_ubyte();
    a_move->extra_string_selector = a_r->read_ubyte();
    a_move->points = a_r->read_ubyte();
    a_r->read_buf(a_move->move_string, 21);
    uint16_t size = a_r->read_uword();
    if (size >= SD_MOVE_FOOTER_STRING_MAX) return SdError::FILE_PARSE_ERROR;
    if (size > 0) {
        a_r->read_buf(a_move->footer_string, size);
        if (a_move->footer_string[size - 1] != 0) return SdError::FILE_PARSE_ERROR;
    }
    if (!a_r->ok()) return SdError::FILE_PARSE_ERROR;
    return SdError::SUCCESS;
}

inline SdError move_save(Writer* a_w, const Move* a_move) {
    if (!a_w || !a_move) return SdError::INVALID_INPUT;
    if (a_move->animation && animation_save(a_w, a_move->animation) != SdError::SUCCESS)
        return SdError::FILE_WRITE_ERROR;
    a_w->write_uword(a_move->ai_opts);
    a_w->write_uword(a_move->pos_constraint);
    a_w->write_ubyte(a_move->unknown_4);
    a_w->write_ubyte(a_move->unknown_5);
    a_w->write_ubyte(a_move->unknown_6);
    a_w->write_ubyte(a_move->unknown_7);
    a_w->write_ubyte(a_move->unknown_8);
    a_w->write_ubyte(a_move->unknown_9);
    a_w->write_ubyte(a_move->unknown_10);
    a_w->write_ubyte(a_move->unknown_11);
    a_w->write_ubyte(a_move->next_anim_id);
    a_w->write_ubyte(a_move->category);
    a_w->write_ubyte(a_move->block_damage);
    a_w->write_ubyte(a_move->block_stun);
    a_w->write_ubyte(a_move->successor_id);
    a_w->write_ubyte(a_move->damage_amount);
    a_w->write_ubyte(a_move->throw_duration);
    a_w->write_ubyte(a_move->extra_string_selector);
    a_w->write_ubyte(a_move->points);
    a_w->write_buf(a_move->move_string, 21);
    size_t len = std::strlen(a_move->footer_string);
    if (len > 0) {
        a_w->write_uword(static_cast<uint16_t>(len + 1));
        a_w->write_buf(a_move->footer_string, len + 1);
    } else {
        a_w->write_uword(0);
    }
    return SdError::SUCCESS;
}

inline Animation* move_get_animation(const Move* a_move) { return a_move ? a_move->animation : nullptr; }

} // namespace formats
} // namespace openomf

#endif // OPENOMF_FORMATS_MOVE_HPP
