#ifndef OPENOMF_FORMATS_BK_HPP
#define OPENOMF_FORMATS_BK_HPP

#include "Error.hpp"
#include "Reader.hpp"
#include "Writer.hpp"
#include "Palette.hpp"
#include "VgaImage.hpp"
#include "Pcx.hpp"
#include "Bkanim.hpp"
#include "../Utils/hpp/Allocator.hpp"
#include <cstring>
#include <cstdint>

namespace openomf {
namespace formats {

constexpr int MAX_BK_ANIMS = 50;
constexpr int MAX_BK_PALETTES = 8;

/** C sd_bk_file 대응. */
struct BkFile {
    uint32_t file_id = 0;
    uint8_t unknown_a = 0;
    uint8_t palette_count = 0;
    BkAnim* anims[MAX_BK_ANIMS] = {};
    VgaImage* background = nullptr;
    VgaPalette* palettes[MAX_BK_PALETTES] = {};
    VgaRemapTables* remaps[MAX_BK_PALETTES] = {};
    char soundtable[30] = {};
};

inline SdError bk_create(BkFile* a_bk) {
    if (!a_bk) return SdError::INVALID_INPUT;
    std::memset(a_bk, 0, sizeof(BkFile));
    return SdError::SUCCESS;
}

inline void bk_free(BkFile* a_bk) {
    if (!a_bk) return;
    for (int i = 0; i < MAX_BK_ANIMS; i++) {
        if (a_bk->anims[i]) {
            bk_anim_free(a_bk->anims[i]);
            openomf::utils::Allocator::free_real(a_bk->anims[i]);
            a_bk->anims[i] = nullptr;
        }
    }
    if (a_bk->background) {
        vga_image_free(a_bk->background);
        omf_free(a_bk->background);
        a_bk->background = nullptr;
    }
    for (int i = 0; i < MAX_BK_PALETTES; i++) {
        if (a_bk->palettes[i]) {
            omf_free(a_bk->palettes[i]);
            a_bk->palettes[i] = nullptr;
        }
        if (a_bk->remaps[i]) {
            omf_free(a_bk->remaps[i]);
            a_bk->remaps[i] = nullptr;
        }
    }
}

/** PCX 파일 하나를 BK로 로드 (배경 + 팔레트 1개). */
inline SdError bk_load_from_pcx(BkFile* a_bk, const char* a_filename) {
    if (!a_bk) return SdError::INVALID_INPUT;
    PcxFile pcx = {};
    SdError ret = pcx_load(&pcx, a_filename);
    if (ret != SdError::SUCCESS) return ret;
    a_bk->file_id = 0;
    a_bk->palette_count = 1;
    a_bk->background = static_cast<VgaImage*>(omf_calloc(1, sizeof(VgaImage)));
    vga_image_copy(a_bk->background, &pcx.image);
    a_bk->palettes[0] = static_cast<VgaPalette*>(omf_malloc(sizeof(VgaPalette)));
    palette_copy(a_bk->palettes[0], &pcx.palette, 0, 256);
    a_bk->remaps[0] = static_cast<VgaRemapTables*>(omf_malloc(sizeof(VgaRemapTables)));
    remaps_init(a_bk->remaps[0]);
    pcx_free(&pcx);
    return SdError::SUCCESS;
}

inline VgaImage* bk_get_background(const BkFile* a_bk) {
    return a_bk ? a_bk->background : nullptr;
}
inline VgaPalette* bk_get_palette(const BkFile* a_bk, int a_index) {
    if (!a_bk || a_index < 0 || a_index >= MAX_BK_PALETTES) return nullptr;
    return a_bk->palettes[a_index];
}
inline BkAnim* bk_get_anim(const BkFile* a_bk, int a_index) {
    if (!a_bk || a_index < 0 || a_index >= MAX_BK_ANIMS) return nullptr;
    return a_bk->anims[a_index];
}

/** Missing 스프라이트: 동일 index를 가진 다른 스프라이트의 data로 채움. */
inline void bk_postprocess(BkFile* a_bk) {
    char* table[1000] = {};
    for (int i = 0; i < MAX_BK_ANIMS; i++) {
        if (!a_bk->anims[i] || !a_bk->anims[i]->animation) continue;
        Animation* anim = a_bk->anims[i]->animation;
        for (int j = 0; j < anim->sprite_count; j++) {
            Sprite* sp = anim->sprites[j];
            if (!sp) continue;
            if (sp->missing > 0) {
                if (sp->index < 1000 && table[sp->index])
                    sp->data = table[sp->index];
            } else {
                if (sp->index < 1000)
                    table[sp->index] = sp->data;
            }
        }
    }
}

/** 전체 .BK 파일 로드. .PCX 확장자면 load_from_pcx. */
inline SdError bk_load(BkFile* a_bk, const char* a_filename) {
    if (!a_bk) return SdError::INVALID_INPUT;
    size_t fn_len = std::strlen(a_filename);
    if (fn_len >= 4 && (std::strncmp(a_filename + fn_len - 4, ".PCX", 4) == 0 ||
                       std::strncmp(a_filename + fn_len - 4, ".pcx", 4) == 0))
        return bk_load_from_pcx(a_bk, a_filename);
    Reader* r = Reader::open(a_filename);
    if (!r) return SdError::FILE_OPEN_ERROR;
    a_bk->file_id = r->read_udword();
    a_bk->unknown_a = r->read_ubyte();
    uint16_t img_w = r->read_uword();
    uint16_t img_h = r->read_uword();
    for (;;) {
        r->skip(4);
        uint8_t animno = r->read_ubyte();
        if (animno >= MAX_BK_ANIMS || !r->ok()) break;
        a_bk->anims[animno] = static_cast<BkAnim*>(omf_calloc(1, sizeof(BkAnim)));
        if (bk_anim_create(a_bk->anims[animno]) != SdError::SUCCESS) { r->close(); return SdError::OUT_OF_MEMORY; }
        if (bk_anim_load(r, a_bk->anims[animno]) != SdError::SUCCESS) { r->close(); return SdError::FILE_PARSE_ERROR; }
    }
    a_bk->background = static_cast<VgaImage*>(omf_calloc(1, sizeof(VgaImage)));
    if (vga_image_create(a_bk->background, img_w, img_h) != SdError::SUCCESS) { r->close(); return SdError::OUT_OF_MEMORY; }
    r->read_buf(a_bk->background->data, static_cast<size_t>(img_w * img_h));
    a_bk->palette_count = r->read_ubyte();
    for (uint8_t i = 0; i < a_bk->palette_count; i++) {
        a_bk->palettes[i] = static_cast<VgaPalette*>(omf_malloc(sizeof(VgaPalette)));
        if (palette_load(r, a_bk->palettes[i]) != SdError::SUCCESS) { r->close(); return SdError::FILE_PARSE_ERROR; }
        a_bk->remaps[i] = static_cast<VgaRemapTables*>(omf_malloc(sizeof(VgaRemapTables)));
        if (palette_remaps_load(r, a_bk->remaps[i]) != SdError::SUCCESS) { r->close(); return SdError::FILE_PARSE_ERROR; }
    }
    r->read_buf(a_bk->soundtable, 30);
    r->close();
    bk_postprocess(a_bk);
    return SdError::SUCCESS;
}

/** 전체 .BK 파일 저장. */
inline SdError bk_save(const BkFile* a_bk, const char* a_filename) {
    if (!a_bk) return SdError::INVALID_INPUT;
    Writer* w = Writer::open(a_filename);
    if (!w) return SdError::FILE_OPEN_ERROR;
    w->write_udword(a_bk->file_id);
    w->write_ubyte(a_bk->unknown_a);
    if (a_bk->background) {
        w->write_uword(static_cast<uint16_t>(a_bk->background->w));
        w->write_uword(static_cast<uint16_t>(a_bk->background->h));
    } else {
        w->write_uword(320);
        w->write_uword(200);
    }
    long rpos = 0;
    for (int i = 0; i < MAX_BK_ANIMS; i++) {
        if (a_bk->anims[i]) {
            long opos = w->pos();
            w->write_udword(0);
            w->write_ubyte(static_cast<uint8_t>(i));
            if (bk_anim_save(w, a_bk->anims[i]) != SdError::SUCCESS) { w->close(); return SdError::FILE_WRITE_ERROR; }
            rpos = w->pos();
            w->seek_start(opos);
            w->write_udword(static_cast<uint32_t>(rpos));
            w->seek_start(rpos);
        }
    }
    w->write_udword(static_cast<uint32_t>(rpos));
    w->write_ubyte(static_cast<uint8_t>(MAX_BK_ANIMS + 1));
    if (a_bk->background)
        w->write_buf(a_bk->background->data, a_bk->background->len);
    else
        w->write_fill(0, 64000);
    w->write_ubyte(a_bk->palette_count);
    for (int i = 0; i < a_bk->palette_count; i++) {
        palette_save(w, a_bk->palettes[i]);
        palette_remaps_save(w, a_bk->remaps[i]);
    }
    w->write_buf(a_bk->soundtable, 30);
    w->close();
    return SdError::SUCCESS;
}

} // namespace formats
} // namespace openomf

#endif // OPENOMF_FORMATS_BK_HPP
