#ifndef OPENOMF_FORMATS_PALETTE_HPP
#define OPENOMF_FORMATS_PALETTE_HPP

#include "Error.hpp"
#include "Reader.hpp"
#include "Writer.hpp"
#include "MemReader.hpp"
#include "MemWriter.hpp"
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <cassert>

namespace openomf {
namespace formats {

/** C video/vga_palette.h vga_color 대응. */
struct VgaColor {
    uint8_t r, g, b;
};
static_assert(sizeof(VgaColor) == 3, "VgaColor 3 bytes");

/** C video/vga_palette.h vga_palette 대응. */
struct VgaPalette {
    VgaColor colors[256];
};
static_assert(sizeof(VgaPalette) == 768, "VgaPalette 768 bytes");

constexpr int VGA_REMAP_COUNT = 19;

/** C video/vga_remap.h vga_remap_table 대응. */
struct VgaRemapTable {
    uint8_t data[256];
};
static_assert(sizeof(VgaRemapTable) == 256, "VgaRemapTable 256 bytes");

/** C video/vga_remap.h vga_remap_tables 대응. */
struct VgaRemapTables {
    VgaRemapTable tables[VGA_REMAP_COUNT];
};
static_assert(sizeof(VgaRemapTables) == 4864, "VgaRemapTables 4864 bytes");

inline void remaps_init(VgaRemapTables* a_remaps) {
    if (a_remaps) std::memset(a_remaps, 0, sizeof(VgaRemapTables));
}

inline uint8_t color_6to8(uint8_t c) { return static_cast<uint8_t>((c << 2) | ((c & 0x30u) >> 4)); }
inline uint8_t color_8to6(uint8_t c) { return static_cast<uint8_t>((c >> 2) & 0x3Fu); }

/** C palette_resolve_color 대응. */
inline uint8_t palette_resolve_color(uint8_t r, uint8_t g, uint8_t b, const VgaPalette* a_pal) {
    for (int i = 0; i < 256; i++) {
        if (a_pal->colors[i].r == r && a_pal->colors[i].g == g && a_pal->colors[i].b == b)
            return static_cast<uint8_t>(i);
    }
    return 0;
}

/** C palette_load_range 대응. */
inline SdError palette_load_range(Reader* a_reader, VgaPalette* a_pal, int a_index_start, int a_index_count) {
    assert(a_index_start + a_index_count <= 256);
    VgaColor d;
    for (int i = a_index_start; i < a_index_start + a_index_count; i++) {
        a_reader->read_buf(reinterpret_cast<char*>(&d), 3);
        a_pal->colors[i].r = color_6to8(d.r);
        a_pal->colors[i].g = color_6to8(d.g);
        a_pal->colors[i].b = color_6to8(d.b);
    }
    return SdError::SUCCESS;
}

/** C palette_mload_range 대응. */
inline SdError palette_mload_range(MemReader* a_reader, VgaPalette* a_pal, int a_index_start, int a_index_count) {
    assert(a_index_start + a_index_count <= 256);
    VgaColor d;
    for (int i = a_index_start; i < a_index_start + a_index_count; i++) {
        a_reader->read_buf(reinterpret_cast<char*>(&d), 3);
        a_pal->colors[i].r = color_6to8(d.r);
        a_pal->colors[i].g = color_6to8(d.g);
        a_pal->colors[i].b = color_6to8(d.b);
    }
    return SdError::SUCCESS;
}

/** C palette_load 대응. */
inline SdError palette_load(Reader* a_reader, VgaPalette* a_pal) {
    return palette_load_range(a_reader, a_pal, 0, 256);
}

/** C palette_remaps_load 대응. */
inline SdError palette_remaps_load(Reader* a_reader, VgaRemapTables* a_remaps) {
    a_reader->read_buf(reinterpret_cast<char*>(a_remaps), static_cast<size_t>(sizeof(VgaRemapTables)));
    return SdError::SUCCESS;
}

/** C palette_save_range 대응. */
inline void palette_save_range(Writer* a_writer, const VgaPalette* a_pal, int a_index_start, int a_index_count) {
    for (int i = a_index_start; i < a_index_start + a_index_count; i++) {
        a_writer->write_ubyte(color_8to6(a_pal->colors[i].r));
        a_writer->write_ubyte(color_8to6(a_pal->colors[i].g));
        a_writer->write_ubyte(color_8to6(a_pal->colors[i].b));
    }
}

/** C palette_msave_range 대응. */
inline void palette_msave_range(MemWriter* a_writer, const VgaPalette* a_pal, int a_index_start, int a_index_count) {
    for (int i = a_index_start; i < a_index_start + a_index_count; i++) {
        a_writer->write_ubyte(color_8to6(a_pal->colors[i].r));
        a_writer->write_ubyte(color_8to6(a_pal->colors[i].g));
        a_writer->write_ubyte(color_8to6(a_pal->colors[i].b));
    }
}

/** C palette_save 대응. */
inline void palette_save(Writer* a_writer, const VgaPalette* a_pal) {
    palette_save_range(a_writer, a_pal, 0, 256);
}

/** C palette_remaps_save 대응. */
inline void palette_remaps_save(Writer* a_writer, const VgaRemapTables* a_remaps) {
    if (a_remaps)
        a_writer->write_buf(reinterpret_cast<const char*>(a_remaps), sizeof(VgaRemapTables));
}

/** C palette_copy 대응. */
inline void palette_copy(VgaPalette* a_dst, const VgaPalette* a_src, int a_index_start, int a_index_count) {
    std::memcpy(&a_dst->colors[a_index_start], &a_src->colors[a_index_start], static_cast<size_t>(a_index_count) * 3);
}

/** C palette_to_gimp_palette 대응. */
inline SdError palette_to_gimp_palette(const VgaPalette* a_pal, const char* a_filename) {
    if (!a_pal || !a_filename) return SdError::INVALID_INPUT;
    Writer* w = Writer::open(a_filename);
    if (!w) return SdError::FILE_OPEN_ERROR;
    w->write_fprintf("GIMP Palette\n");
    w->write_fprintf("Name: %s\n", a_filename);
    w->write_fprintf("#\n");
    for (int i = 0; i < 255; i++) {
        w->write_fprintf("%3u %3u %3u\n",
            a_pal->colors[i].r & 0xffu,
            a_pal->colors[i].g & 0xffu,
            a_pal->colors[i].b & 0xffu);
    }
    w->close();
    return SdError::SUCCESS;
}

/** C palette_from_gimp_palette 대응. */
inline SdError palette_from_gimp_palette(VgaPalette* a_pal, const char* a_filename) {
    if (!a_pal || !a_filename) return SdError::INVALID_INPUT;
    Reader* rd = Reader::open(a_filename);
    if (!rd) return SdError::FILE_OPEN_ERROR;
    char tmp[128];
    if (rd->match("GIMP Palette\n", 13) == 0) {
        rd->close();
        return SdError::FILE_INVALID_TYPE;
    }
    for (;;) {
        if (rd->read_line(tmp, 128) != 0) {
            rd->close();
            return SdError::FILE_INVALID_TYPE;
        }
        if (tmp[0] == '#') break;
    }
    for (int i = 0; i < 255; i++) {
        rd->read_line(tmp, 128);
        tmp[3] = 0;
        tmp[7] = 0;
        tmp[11] = 0;
        a_pal->colors[i].r = static_cast<uint8_t>(std::atoi(tmp + 0));
        a_pal->colors[i].g = static_cast<uint8_t>(std::atoi(tmp + 4));
        a_pal->colors[i].b = static_cast<uint8_t>(std::atoi(tmp + 8));
    }
    rd->close();
    return SdError::SUCCESS;
}

} // namespace formats
} // namespace openomf

#endif // OPENOMF_FORMATS_PALETTE_HPP
