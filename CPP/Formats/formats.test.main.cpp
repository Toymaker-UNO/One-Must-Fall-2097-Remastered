// Formats 모듈 빌드/연결 검증: Reader, Writer, MemReader, MemWriter, Palette.
#include <cstdio>
#include "hpp/Types.hpp"
#include "hpp/Reader.hpp"
#include "hpp/Writer.hpp"
#include "hpp/MemReader.hpp"
#include "hpp/MemWriter.hpp"
#include "hpp/Palette.hpp"
#include "hpp/VgaImage.hpp"
#include "hpp/Pcx.hpp"
#include "hpp/Bk.hpp"
#include "hpp/Move.hpp"
#include "hpp/Af.hpp"

using namespace openomf::formats;

static void test_reader_writer()
{
    const char* tmp = "formats.test.tmp";
    Writer* w = Writer::open(tmp);
    if (!w) throw 0;
    w->write_udword(0x12345678u);
    w->write_ubyte(0xAB);
    w->write_uword(0x1234);
    w->close();

    Reader* r = Reader::open(tmp);
    if (!r) throw 0;
    if (r->read_udword() != 0x12345678u) throw 1;
    if (r->read_ubyte() != 0xAB) throw 2;
    if (r->read_uword() != 0x1234u) throw 3;
    r->close();
    (void)std::remove(tmp);
}

static void test_mem_reader_writer()
{
    char buf[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    MemReader* mr = MemReader::open(buf, 8);
    if (!mr) return;
    if (mr->size() != 8) throw 10;
    if (mr->read_ubyte() != 1) throw 11;
    if (mr->read_udword() != 0x05040302u) throw 12; // little-endian bytes 2,3,4,5
    mr->close();

    MemWriter* mw = MemWriter::open();
    if (!mw) return;
    mw->write_ubyte(0x42);
    mw->write_uword(0xABCD);
    if (mw->pos() != 3) throw 20;
    mw->close();
}

static void test_palette()
{
    VgaPalette pal = {};
    pal.colors[0] = { 255, 0, 0 };
    pal.colors[1] = { 0, 255, 0 };
    if (palette_resolve_color(255, 0, 0, &pal) != 0) throw 30;
    if (palette_resolve_color(0, 255, 0, &pal) != 1) throw 31;
    VgaPalette dst = {};
    palette_copy(&dst, &pal, 0, 2);
    if (dst.colors[0].r != 255 || dst.colors[1].g != 255) throw 32;
}

static void test_vga_image_and_pcx()
{
    VgaImage img = {};
    if (vga_image_create(&img, 10, 10) != SdError::SUCCESS) throw 40;
    if (img.w != 10 || img.h != 10 || img.len != 100) throw 41;
    img.data[0] = 1;
    vga_image_free(&img);

    PcxFile pcx = {};
    if (pcx_load(&pcx, "nonexistent_pcx_12345.pcx") != SdError::FILE_INVALID_TYPE) throw 42;
    pcx_free(&pcx);
}

static void test_bk()
{
    BkFile bk = {};
    if (bk_create(&bk) != SdError::SUCCESS) throw 50;
    if (bk_load_from_pcx(&bk, "nonexistent_bk_pcx.pcx") != SdError::FILE_INVALID_TYPE) throw 51;
    bk_free(&bk);
}

static void test_move_af()
{
    Move mv = {};
    if (move_create(&mv) != SdError::SUCCESS) throw 60;
    move_free(&mv);

    AfFile af = {};
    if (af_create(&af) != SdError::SUCCESS) throw 61;
    af_free(&af);
}

int main()
{
    (void)sizeof(FormatsTypes);
    test_reader_writer();
    test_mem_reader_writer();
    test_palette();
    test_vga_image_and_pcx();
    test_bk();
    test_move_af();
    return 0;
}
