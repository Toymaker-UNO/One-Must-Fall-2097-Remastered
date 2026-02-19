#ifndef OPENOMF_VIDEO_GL3RENDERER_HPP
#define OPENOMF_VIDEO_GL3RENDERER_HPP

/**
 * C video/renderers/opengl3 최소 이식.
 * SDL 창 + OpenGL 3.3 컨텍스트 + clear/swap. draw_surface는 추후 팔레트/텍스처 연동.
 */
#include "hpp/Renderer.hpp"
#include "hpp/Surface.hpp"
#include "ExternalLibrary/hpp/Sdl2.hpp"
#include "ExternalLibrary/hpp/Epoxy.hpp"
#include "Utils/hpp/Allocator.hpp"
#include <SDL2/SDL.h>
#include <vector>
#include <cstring>

namespace openomf {
namespace video {

struct Gl3Context {
    SDL_Window* window = nullptr;
    SDL_GLContext gl_context = nullptr;
    int viewport_w = 0;
    int viewport_h = 0;

    // 간단한 RGBA 텍스처 파이프라인
    GLuint texture = 0;
    int tex_w = 0;
    int tex_h = 0;
    GLuint program = 0;
    GLuint vao = 0;
    GLuint vbo = 0;
};

namespace gl3_detail {

static bool has_gl_available(int major, int minor) {
    external_library::Sdl2::omf_sdl_gl_set_attribute(SDL_GL_CONTEXT_MAJOR_VERSION, major);
    external_library::Sdl2::omf_sdl_gl_set_attribute(SDL_GL_CONTEXT_MINOR_VERSION, minor);
    external_library::Sdl2::omf_sdl_gl_set_attribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_Window* w = external_library::Sdl2::omf_sdl_create_window("", 0, 0, 320, 200,
                                                                   SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL);
    if (!w) return false;
    SDL_GLContext c = external_library::Sdl2::omf_sdl_gl_create_context(w);
    bool ok = (c != nullptr);
    if (c) external_library::Sdl2::omf_sdl_gl_delete_context(c);
    external_library::Sdl2::omf_sdl_destroy_window(w);
    return ok;
}

static bool is_available() {
    return has_gl_available(3, 3);
}

static const char* get_description() {
    return "Hardware OpenGL 3.3 renderer (minimal)";
}

static const char* get_name() {
    return "OpenGL3";
}

static void create_renderer(Renderer* r) {
    if (!r) return;
    Gl3Context* ctx = static_cast<Gl3Context*>(omf_calloc(1, sizeof(Gl3Context)));
    r->ctx = ctx;
}

static void destroy_renderer(Renderer* r) {
    if (!r || !r->ctx) return;
    Gl3Context* ctx = static_cast<Gl3Context*>(r->ctx);
    (void)ctx;  /* close_context 에서 이미 정리됨 */
    omf_free(r->ctx);
    r->ctx = nullptr;
}

// 간단한 셰이더 유틸리티
static GLuint compile_shader(GLenum type, const char* src) {
    using external_library::Epoxy;
    GLuint sh = Epoxy::omf_gl_create_shader(type);
    Epoxy::omf_gl_shader_source(sh, 1, &src, nullptr);
    Epoxy::omf_gl_compile_shader(sh);
    GLint ok = 0;
    Epoxy::omf_gl_get_shader_iv(sh, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        Epoxy::omf_gl_delete_shader(sh);
        return 0;
    }
    return sh;
}

static GLuint create_program_simple() {
    static const char* vs_src =
        "#version 330 core\n"
        "layout(location=0) in vec2 in_pos;\n"
        "layout(location=1) in vec2 in_uv;\n"
        "out vec2 v_uv;\n"
        "void main(){ v_uv = in_uv; gl_Position = vec4(in_pos,0.0,1.0); }\n";
    static const char* fs_src =
        "#version 330 core\n"
        "in vec2 v_uv;\n"
        "out vec4 out_color;\n"
        "uniform sampler2D u_tex;\n"
        "void main(){ out_color = texture(u_tex, v_uv); }\n";

    using external_library::Epoxy;
    GLuint vs = compile_shader(GL_VERTEX_SHADER, vs_src);
    if (!vs) return 0;
    GLuint fs = compile_shader(GL_FRAGMENT_SHADER, fs_src);
    if (!fs) {
        Epoxy::omf_gl_delete_shader(vs);
        return 0;
    }
    GLuint prog = Epoxy::omf_gl_create_program();
    Epoxy::omf_gl_attach_shader(prog, vs);
    Epoxy::omf_gl_attach_shader(prog, fs);
    Epoxy::omf_gl_link_program(prog);
    GLint ok = 0;
    Epoxy::omf_gl_get_program_iv(prog, GL_LINK_STATUS, &ok);
    Epoxy::omf_gl_delete_shader(vs);
    Epoxy::omf_gl_delete_shader(fs);
    if (!ok) {
        Epoxy::omf_gl_delete_program(prog);
        return 0;
    }
    return prog;
}

static bool setup_context(void* userdata, int window_w, int window_h, bool fullscreen, bool vsync,
                          int /*aspect*/, int /*framerate_limit*/) {
    Gl3Context* ctx = static_cast<Gl3Context*>(userdata);
    if (!ctx) return false;

    external_library::Sdl2::omf_sdl_gl_set_attribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    external_library::Sdl2::omf_sdl_gl_set_attribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    external_library::Sdl2::omf_sdl_gl_set_attribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    external_library::Sdl2::omf_sdl_gl_set_attribute(SDL_GL_RED_SIZE, 8);
    external_library::Sdl2::omf_sdl_gl_set_attribute(SDL_GL_GREEN_SIZE, 8);
    external_library::Sdl2::omf_sdl_gl_set_attribute(SDL_GL_BLUE_SIZE, 8);
    external_library::Sdl2::omf_sdl_gl_set_attribute(SDL_GL_ALPHA_SIZE, 8);

    SDL_Window* w = external_library::Sdl2::omf_sdl_create_window(
        "OpenOMF", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_w, window_h,
        SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    if (!w) return false;
    ctx->window = w;

    SDL_GLContext glc = external_library::Sdl2::omf_sdl_gl_create_context(w);
    if (!glc) {
        external_library::Sdl2::omf_sdl_destroy_window(w);
        ctx->window = nullptr;
        return false;
    }
    ctx->gl_context = glc;

    if (fullscreen)
        external_library::Sdl2::omf_sdl_set_window_fullscreen(w, SDL_WINDOW_FULLSCREEN);
    else
        external_library::Sdl2::omf_sdl_set_window_fullscreen(w, 0);
    external_library::Sdl2::omf_sdl_disable_screen_saver();

    if (vsync) {
        if (external_library::Sdl2::omf_sdl_gl_set_swap_interval(-1) != 0)
            external_library::Sdl2::omf_sdl_gl_set_swap_interval(1);
    } else {
        external_library::Sdl2::omf_sdl_gl_set_swap_interval(0);
    }

    external_library::Sdl2::omf_sdl_gl_get_drawable_size(w, &ctx->viewport_w, &ctx->viewport_h);

    using external_library::Epoxy;
    Epoxy::omf_gl_clear_color(0.0f, 0.0f, 0.0f, 1.0f);
    Epoxy::omf_gl_pixel_store_i(GL_UNPACK_ALIGNMENT, 1);
    Epoxy::omf_gl_pixel_store_i(GL_PACK_ALIGNMENT, 1);

    // 간단한 풀스크린 사각형 설정
    ctx->program = create_program_simple();
    if (!ctx->program) return false;

    Epoxy::omf_gl_gen_vertex_arrays(1, &ctx->vao);
    Epoxy::omf_gl_gen_buffers(1, &ctx->vbo);
    Epoxy::omf_gl_bind_vertex_array(ctx->vao);
    Epoxy::omf_gl_bind_buffer(GL_ARRAY_BUFFER, ctx->vbo);

    // pos(x,y), uv(u,v) * 6 정점 (두 삼각형)
    const float verts[] = {
        -1.0f, -1.0f, 0.0f, 1.0f,
         1.0f, -1.0f, 1.0f, 1.0f,
         1.0f,  1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f, 1.0f,
         1.0f,  1.0f, 1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f, 0.0f,
    };
    Epoxy::omf_gl_buffer_data(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    Epoxy::omf_gl_vertex_attrib_pointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<const void*>(0));
    Epoxy::omf_gl_enable_vertex_attrib_array(0);
    Epoxy::omf_gl_vertex_attrib_pointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                                        reinterpret_cast<const void*>(2 * sizeof(float)));
    Epoxy::omf_gl_enable_vertex_attrib_array(1);

    Epoxy::omf_gl_bind_vertex_array(0);
    Epoxy::omf_gl_bind_buffer(GL_ARRAY_BUFFER, 0);

    // 텍스처 생성은 draw_surface 시점에
    return true;
}

static void get_context_state(void* userdata, int* window_w, int* window_h, bool* /*fullscreen*/,
                              bool* /*vsync*/, int* /*aspect*/) {
    Gl3Context* ctx = static_cast<Gl3Context*>(userdata);
    if (window_w) *window_w = ctx ? ctx->viewport_w : 0;
    if (window_h) *window_h = ctx ? ctx->viewport_h : 0;
}

static bool reset_context_with(void* /*userdata*/, int /*window_w*/, int /*window_h*/,
                               bool /*fullscreen*/, bool /*vsync*/, int /*aspect*/, int /*framerate_limit*/) {
    return true;
}

static void reset_context(void* /*userdata*/) {}

static void close_context(void* userdata) {
    Gl3Context* ctx = static_cast<Gl3Context*>(userdata);
    if (!ctx) return;
    using external_library::Epoxy;

    if (ctx->texture) {
        Epoxy::omf_gl_delete_textures(1, &ctx->texture);
        ctx->texture = 0;
    }
    if (ctx->vbo) {
        Epoxy::omf_gl_delete_buffers(1, &ctx->vbo);
        ctx->vbo = 0;
    }
    if (ctx->vao) {
        Epoxy::omf_gl_delete_vertex_arrays(1, &ctx->vao);
        ctx->vao = 0;
    }
    if (ctx->program) {
        Epoxy::omf_gl_delete_program(ctx->program);
        ctx->program = 0;
    }
    if (ctx->gl_context && ctx->window) {
        external_library::Sdl2::omf_sdl_gl_delete_context(ctx->gl_context);
        ctx->gl_context = nullptr;
    }
    if (ctx->window) {
        external_library::Sdl2::omf_sdl_destroy_window(ctx->window);
        ctx->window = nullptr;
    }
    ctx->viewport_w = 0;
    ctx->viewport_h = 0;
    ctx->tex_w = 0;
    ctx->tex_h = 0;
}

static void draw_surface(void* userdata, const Surface* src_surface, SDL_Rect* /*rect*/,
                         int /*remap_offset*/, int /*remap_rounds*/, int /*palette_offset*/,
                         int /*palette_limit*/, int /*opacity*/, unsigned int /*flip_mode*/,
                         unsigned int /*options*/) {
    Gl3Context* ctx = static_cast<Gl3Context*>(userdata);
    if (!ctx || !src_surface || !src_surface->data || src_surface->w <= 0 || src_surface->h <= 0)
        return;

    using external_library::Epoxy;

    // 팔레트 인덱스 → RGBA 변환
    const int w = src_surface->w;
    const int h = src_surface->h;
    const formats::VgaPalette* pal = src_surface->palette;
    std::vector<unsigned char> rgba(static_cast<size_t>(w) * h * 4);
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            const int idx = y * w + x;
            const unsigned char pi = src_surface->data[idx];
            unsigned char r = 0, g = 0, b = 0;
            if (pal) {
                r = pal->colors[pi].r;
                g = pal->colors[pi].g;
                b = pal->colors[pi].b;
            } else {
                // 팔레트가 없으면 단순 회색조
                r = g = b = static_cast<unsigned char>(pi * 4);
            }
            rgba[idx * 4 + 0] = r;
            rgba[idx * 4 + 1] = g;
            rgba[idx * 4 + 2] = b;
            rgba[idx * 4 + 3] = 255;
        }
    }

    // 텍스처 생성/리사이즈
    if (!ctx->texture) {
        Epoxy::omf_gl_gen_textures(1, &ctx->texture);
        ctx->tex_w = 0;
        ctx->tex_h = 0;
    }
    Epoxy::omf_gl_bind_texture(GL_TEXTURE_2D, ctx->texture);
    if (ctx->tex_w != w || ctx->tex_h != h) {
        ctx->tex_w = w;
        ctx->tex_h = h;
        Epoxy::omf_gl_tex_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        Epoxy::omf_gl_tex_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        Epoxy::omf_gl_tex_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        Epoxy::omf_gl_tex_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        Epoxy::omf_gl_tex_image2_d(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba.data());
    } else {
        Epoxy::omf_gl_tex_sub_image2_d(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, rgba.data());
    }

    // 풀스크린 quad 그리기 (배경만 우선)
    Epoxy::omf_gl_use_program(ctx->program);
    GLint loc = Epoxy::omf_gl_get_uniform_location(ctx->program, "u_tex");
    if (loc >= 0) Epoxy::omf_gl_uniform1i(loc, 0);
    Epoxy::omf_gl_bind_vertex_array(ctx->vao);
    Epoxy::omf_gl_draw_arrays(GL_TRIANGLES, 0, 6);
    Epoxy::omf_gl_bind_vertex_array(0);
}

static void move_target(void* /*ctx*/, int /*x*/, int /*y*/) {}

static void render_prepare(void* userdata, unsigned /*framebuffer_options*/) {
    Gl3Context* ctx = static_cast<Gl3Context*>(userdata);
    if (!ctx) return;
    external_library::Epoxy::omf_gl_viewport(0, 0, ctx->viewport_w, ctx->viewport_h);
    external_library::Epoxy::omf_gl_clear(GL_COLOR_BUFFER_BIT);
}

static void render_finish(void* userdata) {
    Gl3Context* ctx = static_cast<Gl3Context*>(userdata);
    if (ctx && ctx->window)
        external_library::Sdl2::omf_sdl_gl_swap_window(ctx->window);
}

static void render_area_prepare(void* /*ctx*/, const SDL_Rect* /*area*/) {}
static void render_area_finish(void* /*ctx*/, Surface* /*dst*/) {}
static void capture_screen(void* /*ctx*/, VideoScreenshotSignal /*screenshot_cb*/) {}
static void signal_scene_change(void* /*ctx*/) {}
static void signal_draw_atlas(void* /*ctx*/, bool /*toggle*/) {}

} // namespace gl3_detail

static void gl3_create(Renderer* re) { gl3_detail::create_renderer(re); }
static void gl3_destroy(Renderer* re) { gl3_detail::destroy_renderer(re); }

inline void gl3_renderer_set_callbacks(Renderer* r) {
    if (!r) return;
    std::memset(r, 0, sizeof(Renderer));
    r->is_available = gl3_detail::is_available;
    r->get_description = gl3_detail::get_description;
    r->get_name = gl3_detail::get_name;
    r->create = gl3_create;
    r->destroy = gl3_destroy;
    r->setup_context = gl3_detail::setup_context;
    r->get_context_state = gl3_detail::get_context_state;
    r->reset_context_with = gl3_detail::reset_context_with;
    r->reset_context = gl3_detail::reset_context;
    r->close_context = gl3_detail::close_context;
    r->draw_surface = gl3_detail::draw_surface;
    r->move_target = gl3_detail::move_target;
    r->render_prepare = gl3_detail::render_prepare;
    r->render_finish = gl3_detail::render_finish;
    r->render_area_prepare = gl3_detail::render_area_prepare;
    r->render_area_finish = gl3_detail::render_area_finish;
    r->capture_screen = gl3_detail::capture_screen;
    r->signal_scene_change = gl3_detail::signal_scene_change;
    r->signal_draw_atlas = gl3_detail::signal_draw_atlas;
    r->ctx = nullptr;
}

} // namespace video
} // namespace openomf

#endif // OPENOMF_VIDEO_GL3RENDERER_HPP
