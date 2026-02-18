#ifndef OPENOMF_FORMATS_PCX_HPP
#define OPENOMF_FORMATS_PCX_HPP

#include "Error.hpp"
#include "Reader.hpp"
#include "VgaImage.hpp"
#include "Palette.hpp"
#include <cstdint>
#include <cstring>

namespace openomf {
namespace formats {

/** C pcx_file 대응. PCX 헤더 + 이미지 + 팔레트. */
struct PcxFile {
    uint8_t manufacturer = 0;
    uint8_t version = 0;
    uint8_t encoding = 0;
    uint8_t bits_per_plane = 0;
    uint16_t window_x_min = 0;
    uint16_t window_y_min = 0;
    uint16_t window_x_max = 0;
    uint16_t window_y_max = 0;
    uint16_t horz_dpi = 0;
    uint16_t vert_dpi = 0;
    uint8_t header_palette[48] = {};
    uint8_t reserved = 0;
    uint8_t color_planes = 0;
    uint16_t bytes_per_plane_line = 0;
    uint16_t palette_info = 0;
    uint16_t hor_scr_size = 0;
    uint16_t ver_scr_size = 0;
    VgaImage image = {};
    VgaPalette palette = {};
};

struct PcxFontGlyph {
    uint16_t x = 0;
    uint8_t y = 0;
    uint8_t width = 0;
};

struct PcxFont {
    PcxFile pcx = {};
    uint8_t glyph_height = 0;
    uint8_t glyph_count = 0;
    PcxFontGlyph glyphs[256] = {};
};

/** RLE 디코드: 다음 1바이트 또는 반복. 반환: 쓴 바이트 수. */
inline unsigned decode_next_bytes(char* a_dest, Reader* a_reader) {
    uint8_t fst = a_reader->read_ubyte();
    unsigned repeat = 0;
    if (fst >= 192) repeat = fst - 192;
    if (repeat == 0) {
        a_dest[0] = static_cast<char>(fst);
        return 1;
    }
    uint8_t snd = a_reader->read_ubyte();
    for (unsigned i = 0; i < repeat; i++)
        a_dest[i] = static_cast<char>(snd);
    return repeat;
}

inline SdError pcx_load(PcxFile* a_pcx, const char* a_filename) {
    std::memset(a_pcx, 0, sizeof(*a_pcx));
    Reader* reader = Reader::open(a_filename);
    if (!reader) return SdError::FILE_INVALID_TYPE;
    long filesize = reader->filesize();
    if (filesize <= 128) {
        reader->close();
        return SdError::FILE_INVALID_TYPE;
    }
    a_pcx->manufacturer = reader->read_ubyte();
    a_pcx->version = reader->read_ubyte();
    a_pcx->encoding = reader->read_ubyte();
    a_pcx->bits_per_plane = reader->read_ubyte();
    a_pcx->window_x_min = reader->read_uword();
    a_pcx->window_y_min = reader->read_uword();
    a_pcx->window_x_max = reader->read_uword();
    a_pcx->window_y_max = reader->read_uword();
    a_pcx->horz_dpi = reader->read_uword();
    a_pcx->vert_dpi = reader->read_uword();
    if (reader->read_buf(reinterpret_cast<char*>(a_pcx->header_palette), 48) == 0) {
        reader->close();
        return SdError::FILE_READ_ERROR;
    }
    a_pcx->reserved = reader->read_ubyte();
    a_pcx->color_planes = reader->read_ubyte();
    a_pcx->bytes_per_plane_line = reader->read_uword();
    a_pcx->palette_info = reader->read_uword();
    a_pcx->hor_scr_size = reader->read_uword();
    a_pcx->ver_scr_size = reader->read_uword();
    if (reader->set_pos(128) != 1) {
        reader->close();
        return SdError::FILE_INVALID_TYPE;
    }
    SdError ret = vga_image_create(&a_pcx->image, 320, 200);
    if (ret != SdError::SUCCESS) {
        reader->close();
        return ret;
    }
    for (unsigned j = 0; j < 200; j++) {
        for (unsigned i = 0; i < 320; )
            i += decode_next_bytes(&a_pcx->image.data[(320 * j) + i], reader);
    }
    if (reader->read_ubyte() != 0x0c) {
        reader->close();
        vga_image_free(&a_pcx->image);
        return SdError::FILE_READ_ERROR;
    }
    if (reader->read_buf(reinterpret_cast<char*>(&a_pcx->palette.colors), sizeof(VgaPalette)) == 0) {
        reader->close();
        vga_image_free(&a_pcx->image);
        return SdError::FILE_READ_ERROR;
    }
    reader->close();
    return SdError::SUCCESS;
}

inline SdError pcx_load_font(PcxFont* a_font, const char* a_filename) {
    std::memset(a_font, 0, sizeof(PcxFont));
    SdError ret = pcx_load(&a_font->pcx, a_filename);
    if (ret != SdError::SUCCESS) return ret;
    char corner_color = a_font->pcx.image.data[0];
    if (a_font->pcx.image.data[1] != corner_color || a_font->pcx.image.data[320] != corner_color ||
        a_font->pcx.image.data[321] == corner_color) {
        vga_image_free(&a_font->pcx.image);
        return SdError::FORMAT_NOT_SUPPORTED;
    }
    a_font->glyph_height = 0;
    for (int i = 1; i < 200; i++) {
        if (a_font->pcx.image.data[(320 * i) + 1] == corner_color) break;
        a_font->glyph_height++;
    }
    int glyph = 0;
    for (int i = 1; i < 200 && a_font->pcx.image.data[(320 * (i + a_font->glyph_height)) + 1] == corner_color;
         i += a_font->glyph_height + 1) {
        int width = 0;
        for (int j = 1; j < 320; j++) {
            if (a_font->pcx.image.data[(320 * i) + j] == corner_color) {
                a_font->glyphs[glyph].width = static_cast<uint8_t>(width);
                a_font->glyphs[glyph].x = static_cast<uint16_t>(j - width);
                a_font->glyphs[glyph].y = static_cast<uint8_t>(i);
                glyph++;
                width = 0;
            } else {
                width++;
            }
        }
    }
    a_font->glyph_count = static_cast<uint8_t>(glyph);
    return SdError::SUCCESS;
}

inline SdError pcx_font_decode(const PcxFont* a_font, VgaImage* a_o, uint8_t a_ch, int8_t a_palette_offset) {
    if (!a_font || !a_o || a_ch >= a_font->glyph_count) return SdError::INVALID_INPUT;
    int k = 0;
    for (int i = a_font->glyphs[a_ch].y; i < a_font->glyphs[a_ch].y + a_font->glyph_height; i++) {
        int l = 0;
        for (int j = a_font->glyphs[a_ch].x; j < a_font->glyphs[a_ch].x + a_font->glyphs[a_ch].width; j++) {
            uint8_t pix = static_cast<uint8_t>(a_font->pcx.image.data[(i * 320) + j]);
            a_o->data[(k * a_font->glyphs[a_ch].width) + l] = pix ? static_cast<char>(a_palette_offset + pix) : 0;
            l++;
        }
        k++;
    }
    return SdError::SUCCESS;
}

inline void pcx_free(PcxFile* a_pcx) {
    if (!a_pcx) return;
    vga_image_free(&a_pcx->image);
}

inline void pcx_font_free(PcxFont* a_font) {
    if (!a_font) return;
    vga_image_free(&a_font->pcx.image);
}

} // namespace formats
} // namespace openomf

#endif // OPENOMF_FORMATS_PCX_HPP
