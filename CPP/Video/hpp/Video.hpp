#ifndef OPENOMF_VIDEO_VIDEO_HPP
#define OPENOMF_VIDEO_VIDEO_HPP

#include "hpp/Renderer.hpp"
#include "hpp/NullRenderer.hpp"
#include "hpp/Surface.hpp"
#include <SDL2/SDL.h>
#include <cstring>
#include <cstddef>

namespace openomf {
namespace video {

constexpr int NATIVE_W = 320;
constexpr int NATIVE_H = 200;

constexpr int MAX_AVAILABLE_RENDERERS = 8;

struct AvailableRenderer {
    void (*set_callbacks)(Renderer* r) = nullptr;
    const char* name = nullptr;
    const char* description = nullptr;
};

inline static AvailableRenderer s_available[MAX_AVAILABLE_RENDERERS];
inline static int s_renderer_count = 0;
inline static Renderer s_current = {};

inline void video_scan_renderers() {
    s_renderer_count = 0;
    Renderer tmp = {};
    null_renderer_set_callbacks(&tmp);
    if (tmp.is_available && tmp.is_available()) {
        s_available[s_renderer_count].set_callbacks = null_renderer_set_callbacks;
        s_available[s_renderer_count].name = tmp.get_name ? tmp.get_name() : "";
        s_available[s_renderer_count].description = tmp.get_description ? tmp.get_description() : "";
        s_renderer_count++;
    }
}

inline int video_get_renderer_count() {
    return s_renderer_count;
}

inline bool video_get_renderer_info(int index, const char** name, const char** description) {
    if (index < 0 || index >= s_renderer_count) return false;
    if (name) *name = s_available[index].name;
    if (description) *description = s_available[index].description;
    return true;
}

inline bool video_find_renderer(const char* try_name) {
    if (try_name && *try_name != '\0') {
        for (int i = 0; i < s_renderer_count; i++) {
            if (s_available[i].name && std::strcmp(s_available[i].name, try_name) == 0) {
                s_available[i].set_callbacks(&s_current);
                return true;
            }
        }
    }
    if (s_renderer_count > 0) {
        s_available[0].set_callbacks(&s_current);
        return true;
    }
    std::memset(&s_current, 0, sizeof(Renderer));
    return false;
}

/** C video_init. try_name can be nullptr to pick first available. */
inline bool video_init(const char* try_name, int window_w, int window_h, bool fullscreen, bool vsync, int aspect, int framerate_limit) {
    (void)window_w;
    (void)window_h;
    (void)fullscreen;
    (void)vsync;
    (void)aspect;
    (void)framerate_limit;
    if (!video_find_renderer(try_name)) return false;
    if (s_current.create) s_current.create(&s_current);
    if (s_current.setup_context && !s_current.setup_context(s_current.ctx, window_w, window_h, fullscreen, vsync, aspect, framerate_limit)) {
        if (s_current.destroy) s_current.destroy(&s_current);
        return false;
    }
    return true;
}

/** C video_close. */
inline void video_close() {
    if (s_current.close_context) s_current.close_context(s_current.ctx);
    if (s_current.destroy) s_current.destroy(&s_current);
    std::memset(&s_current, 0, sizeof(Renderer));
}

/** C video_render_prepare. 현재 렌더러에 prepare 위임. */
inline void video_render_prepare(unsigned framebuffer_options) {
    if (s_current.render_prepare)
        s_current.render_prepare(s_current.ctx, framebuffer_options);
}

/** C video_render_finish. 현재 렌더러에 finish 위임. */
inline void video_render_finish() {
    if (s_current.render_finish)
        s_current.render_finish(s_current.ctx);
}

/** C video_draw. 배경/스프라이트 단순 그리기. */
inline void video_draw(const Surface* src_surface, int x, int y) {
    if (!src_surface || !s_current.draw_surface) return;
    SDL_Rect dst = { x, y, src_surface->w, src_surface->h };
    s_current.draw_surface(s_current.ctx, src_surface, &dst, 0, 0, 0, 255, 255, 0, 0);
}

} // namespace video
} // namespace openomf

#endif // OPENOMF_VIDEO_VIDEO_HPP
