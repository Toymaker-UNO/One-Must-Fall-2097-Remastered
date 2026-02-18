#ifndef OPENOMF_FORMATS_BKANIM_HPP
#define OPENOMF_FORMATS_BKANIM_HPP

#include "Error.hpp"
#include "Animation.hpp"
#include "Reader.hpp"
#include "Writer.hpp"
#include "../Utils/hpp/Allocator.hpp"
#include <cstring>
#include <cstdint>

namespace openomf {
namespace formats {

constexpr int SD_BK_FOOTER_STRING_MAX = 512;

/** C sd_bk_anim 대응. */
struct BkAnim {
    uint8_t null = 0;
    uint8_t chain_hit = 0;
    uint8_t chain_no_hit = 0;
    uint8_t load_on_start = 0;
    uint16_t probability = 0;
    uint8_t hazard_damage = 0;
    char footer_string[SD_BK_FOOTER_STRING_MAX] = {};
    Animation* animation = nullptr;
};

inline SdError bk_anim_create(BkAnim* a_bka) {
    if (!a_bka) return SdError::INVALID_INPUT;
    std::memset(a_bka, 0, sizeof(BkAnim));
    return SdError::SUCCESS;
}

inline SdError bk_anim_copy(BkAnim* a_dst, const BkAnim* a_src) {
    if (!a_dst || !a_src) return SdError::INVALID_INPUT;
    std::memset(a_dst, 0, sizeof(BkAnim));
    a_dst->null = a_src->null;
    a_dst->chain_hit = a_src->chain_hit;
    a_dst->chain_no_hit = a_src->chain_no_hit;
    a_dst->load_on_start = a_src->load_on_start;
    a_dst->probability = a_src->probability;
    a_dst->hazard_damage = a_src->hazard_damage;
    std::strncpy(a_dst->footer_string, a_src->footer_string, SD_BK_FOOTER_STRING_MAX - 1);
    a_dst->footer_string[SD_BK_FOOTER_STRING_MAX - 1] = '\0';
    if (a_src->animation) {
        a_dst->animation = static_cast<Animation*>(omf_calloc(1, sizeof(Animation)));
        animation_copy(a_dst->animation, a_src->animation);
    }
    return SdError::SUCCESS;
}

inline void bk_anim_free(BkAnim* a_bka) {
    if (!a_bka) return;
    if (a_bka->animation) {
        animation_free(a_bka->animation);
        openomf::utils::Allocator::free_real(a_bka->animation);
        a_bka->animation = nullptr;
    }
}

inline SdError bk_anim_load(Reader* a_r, BkAnim* a_bka) {
    if (!a_r || !a_bka) return SdError::INVALID_INPUT;
    a_bka->null = a_r->read_ubyte();
    a_bka->chain_hit = a_r->read_ubyte();
    a_bka->chain_no_hit = a_r->read_ubyte();
    a_bka->load_on_start = a_r->read_ubyte();
    a_bka->probability = a_r->read_uword();
    a_bka->hazard_damage = a_r->read_ubyte();
    uint16_t size = a_r->read_uword();
    if (size >= SD_BK_FOOTER_STRING_MAX) return SdError::FILE_PARSE_ERROR;
    if (size > 0) {
        a_r->read_buf(a_bka->footer_string, size);
        if (a_bka->footer_string[size - 1] != 0) return SdError::FILE_PARSE_ERROR;
    }
    a_bka->animation = static_cast<Animation*>(omf_calloc(1, sizeof(Animation)));
    if (animation_create(a_bka->animation) != SdError::SUCCESS) return SdError::OUT_OF_MEMORY;
    if (animation_load(a_r, a_bka->animation) != SdError::SUCCESS) return SdError::FILE_PARSE_ERROR;
    return SdError::SUCCESS;
}

inline SdError bk_anim_save(Writer* a_w, const BkAnim* a_bka) {
    if (!a_w || !a_bka) return SdError::INVALID_INPUT;
    a_w->write_ubyte(a_bka->null);
    a_w->write_ubyte(a_bka->chain_hit);
    a_w->write_ubyte(a_bka->chain_no_hit);
    a_w->write_ubyte(a_bka->load_on_start);
    a_w->write_uword(a_bka->probability);
    a_w->write_ubyte(a_bka->hazard_damage);
    size_t len = std::strlen(a_bka->footer_string);
    if (len > 0) {
        a_w->write_uword(static_cast<uint16_t>(len + 1));
        a_w->write_buf(a_bka->footer_string, len + 1);
    } else {
        a_w->write_uword(0);
    }
    if (a_bka->animation && animation_save(a_w, a_bka->animation) != SdError::SUCCESS)
        return SdError::FILE_WRITE_ERROR;
    return SdError::SUCCESS;
}

inline Animation* bk_anim_get_animation(const BkAnim* a_bka) { return a_bka ? a_bka->animation : nullptr; }

} // namespace formats
} // namespace openomf

#endif // OPENOMF_FORMATS_BKANIM_HPP
