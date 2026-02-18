#ifndef OPENOMF_FORMATS_AF_HPP
#define OPENOMF_FORMATS_AF_HPP

#include "Error.hpp"
#include "Reader.hpp"
#include "Writer.hpp"
#include "Move.hpp"
#include "../Utils/hpp/Allocator.hpp"
#include <cstring>
#include <cstdint>

namespace openomf {
namespace formats {

constexpr int MAX_AF_MOVES = 70;

/** C sd_af_file 대응. HAR(로봇) 데이터. */
struct AfFile {
    uint16_t file_id = 0;
    uint16_t exec_window = 0;
    float endurance = 0.f;
    uint8_t unknown_b = 0;
    uint16_t health = 0;
    float forward_speed = 0.f, reverse_speed = 0.f, jump_speed = 0.f, fall_speed = 0.f;
    uint8_t unknown_c = 0, unknown_d = 0;
    Move* moves[MAX_AF_MOVES] = {};
    char soundtable[30] = {};
};

inline SdError af_create(AfFile* a_af) {
    if (!a_af) return SdError::INVALID_INPUT;
    std::memset(a_af, 0, sizeof(AfFile));
    return SdError::SUCCESS;
}

inline SdError af_copy(AfFile* a_dst, const AfFile* a_src) {
    if (!a_dst || !a_src) return SdError::INVALID_INPUT;
    std::memset(a_dst, 0, sizeof(AfFile));
    a_dst->file_id = a_src->file_id;
    a_dst->exec_window = a_src->exec_window;
    a_dst->endurance = a_src->endurance;
    a_dst->unknown_b = a_src->unknown_b;
    a_dst->health = a_src->health;
    a_dst->forward_speed = a_src->forward_speed;
    a_dst->reverse_speed = a_src->reverse_speed;
    a_dst->jump_speed = a_src->jump_speed;
    a_dst->fall_speed = a_src->fall_speed;
    a_dst->unknown_c = a_src->unknown_c;
    a_dst->unknown_d = a_src->unknown_d;
    std::memcpy(a_dst->soundtable, a_src->soundtable, 30);
    for (int i = 0; i < MAX_AF_MOVES; i++) {
        if (a_src->moves[i]) {
            a_dst->moves[i] = static_cast<Move*>(omf_calloc(1, sizeof(Move)));
            move_copy(a_dst->moves[i], a_src->moves[i]);
        }
    }
    return SdError::SUCCESS;
}

inline void af_free(AfFile* a_af) {
    if (!a_af) return;
    for (int i = 0; i < MAX_AF_MOVES; i++) {
        if (a_af->moves[i]) {
            move_free(a_af->moves[i]);
            openomf::utils::Allocator::free_real(a_af->moves[i]);
            a_af->moves[i] = nullptr;
        }
    }
}

inline void af_postprocess(AfFile* a_af) {
    char* table[1000] = {};
    for (int i = 0; i < MAX_AF_MOVES; i++) {
        if (!a_af->moves[i] || !a_af->moves[i]->animation) continue;
        Animation* anim = a_af->moves[i]->animation;
        for (int j = 0; j < anim->sprite_count; j++) {
            Sprite* sp = anim->sprites[j];
            if (!sp) continue;
            if (sp->missing > 0) {
                unsigned idx = sp->index;
                if (idx < 1000 && table[idx]) sp->data = table[idx];
            } else {
                unsigned idx = sp->index;
                if (idx < 1000) table[idx] = sp->data;
            }
        }
    }
}

inline SdError af_load(AfFile* a_af, const char* a_filename) {
    if (!a_af) return SdError::INVALID_INPUT;
    Reader* r = Reader::open(a_filename);
    if (!r) return SdError::FILE_OPEN_ERROR;
    a_af->file_id = r->read_uword();
    a_af->exec_window = r->read_uword();
    a_af->endurance = r->read_udword() / 256.0f;
    a_af->unknown_b = r->read_ubyte();
    a_af->health = r->read_uword();
    a_af->forward_speed = r->read_dword() / 256.0f;
    a_af->reverse_speed = r->read_dword() / 256.0f;
    a_af->jump_speed = r->read_dword() / 256.0f;
    a_af->fall_speed = r->read_dword() / 256.0f;
    a_af->unknown_c = r->read_ubyte();
    a_af->unknown_d = r->read_ubyte();
    for (;;) {
        uint8_t moveno = r->read_ubyte();
        if (moveno >= MAX_AF_MOVES || !r->ok()) break;
        a_af->moves[moveno] = static_cast<Move*>(omf_calloc(1, sizeof(Move)));
        if (move_create(a_af->moves[moveno]) != SdError::SUCCESS) { r->close(); return SdError::OUT_OF_MEMORY; }
        if (move_load(r, a_af->moves[moveno]) != SdError::SUCCESS) { r->close(); return SdError::FILE_PARSE_ERROR; }
    }
    r->read_buf(a_af->soundtable, 30);
    r->close();
    af_postprocess(a_af);
    return SdError::SUCCESS;
}

inline SdError af_save(const AfFile* a_af, const char* a_filename) {
    if (!a_af) return SdError::INVALID_INPUT;
    Writer* w = Writer::open(a_filename);
    if (!w) return SdError::FILE_OPEN_ERROR;
    w->write_uword(a_af->file_id);
    w->write_uword(a_af->exec_window);
    w->write_udword(static_cast<uint32_t>(a_af->endurance * 256));
    w->write_ubyte(a_af->unknown_b);
    w->write_uword(a_af->health);
    w->write_dword(static_cast<int32_t>(a_af->forward_speed * 256));
    w->write_dword(static_cast<int32_t>(a_af->reverse_speed * 256));
    w->write_dword(static_cast<int32_t>(a_af->jump_speed * 256));
    w->write_dword(static_cast<int32_t>(a_af->fall_speed * 256));
    w->write_ubyte(a_af->unknown_c);
    w->write_ubyte(a_af->unknown_d);
    for (int i = 0; i < MAX_AF_MOVES; i++) {
        if (a_af->moves[i]) {
            w->write_ubyte(static_cast<uint8_t>(i));
            if (move_save(w, a_af->moves[i]) != SdError::SUCCESS) { w->close(); return SdError::FILE_WRITE_ERROR; }
        }
    }
    w->write_ubyte(250);
    w->write_buf(a_af->soundtable, 30);
    w->close();
    return SdError::SUCCESS;
}

inline Move* af_get_move(const AfFile* a_af, int a_index) {
    if (!a_af || a_index < 0 || a_index >= MAX_AF_MOVES) return nullptr;
    return a_af->moves[a_index];
}

} // namespace formats
} // namespace openomf

#endif // OPENOMF_FORMATS_AF_HPP
