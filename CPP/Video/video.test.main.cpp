// Video 모듈 빌드/연결 검증: Enums, Color, Image, Surface, Renderer, NullRenderer, Video
#include "hpp/Enums.hpp"
#include "hpp/Color.hpp"
#include "hpp/Image.hpp"
#include "hpp/Surface.hpp"
#include "hpp/Video.hpp"

using namespace openomf::video;

static void test_enums_color()
{
    if (NATIVE_W != 320 || NATIVE_H != 200) throw 1;
    (void)SPRITE_REMAP;
    (void)FBUFOPT_CREDITS;
    (void)FLIP_NONE;
    if (COLOR_GREEN != 0xA7) throw 2;
    Color c = { 1, 2, 3 };
    if (c.r != 1 || c.g != 2 || c.b != 3) throw 3;
}

static void test_image()
{
    Image img = {};
    if (image_create(&img, 10, 10) != 0) throw 10;
    if (img.w != 10 || img.h != 10 || !img.data) throw 11;
    image_clear(&img, 0);
    image_free(&img);
    if (img.data != nullptr) throw 12;
}

static void test_surface()
{
    Surface sur = {};
    surface_create(&sur, 16, 16);
    if (sur.w != 16 || sur.h != 16 || !sur.data) throw 20;
    surface_clear(&sur);
    surface_set_transparency(&sur, 0);
    surface_free(&sur);
}

static void test_surface_from_vga()
{
    openomf::formats::VgaImage vga = {};
    if (openomf::formats::vga_image_create(&vga, 8, 8) != openomf::formats::SdError::SUCCESS) throw 30;
    vga.data[0] = 42;
    Surface sur = {};
    surface_create_from_vga(&sur, &vga);
    if (sur.w != 8 || sur.h != 8 || sur.data[0] != 42) throw 31;
    surface_free(&sur);
    openomf::formats::vga_image_free(&vga);
}

static void test_video()
{
    video_scan_renderers();
    if (video_get_renderer_count() < 1) throw 40;
    const char* name = nullptr;
    const char* desc = nullptr;
    if (!video_get_renderer_info(0, &name, &desc)) throw 41;
    if (!name || !desc) throw 42;
    if (!video_init(nullptr, 320, 200, false, false, 0, 0)) throw 43;
    video_close();
}

int main()
{
    test_enums_color();
    test_image();
    test_surface();
    test_surface_from_vga();
    test_video();
    return 0;
}
