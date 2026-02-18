#ifndef OPENOMF_VIDEO_NULLRENDERER_HPP
#define OPENOMF_VIDEO_NULLRENDERER_HPP

#include "hpp/Renderer.hpp"

namespace openomf {
namespace video {

namespace detail {
static bool null_is_available(void) { return true; }
static const char* null_get_description(void) { return "Zero output renderer"; }
static const char* null_get_name(void) { return "NULL"; }
static void null_create(Renderer*) {}
static void null_destroy(Renderer*) {}
static bool null_setup_context(void*, int, int, bool, bool, int, int) { return true; }
static void null_get_context_state(void* ctx, int* w, int* h, bool* fs, bool* vs, int* a) {
    (void)ctx;
    if (w) *w = 0;
    if (h) *h = 0;
    if (fs) *fs = false;
    if (vs) *vs = false;
    if (a) *a = 0;
}
static bool null_reset_context_with(void*, int, int, bool, bool, int, int) { return true; }
static void null_reset_context(void*) {}
static void null_close_context(void*) {}
static void null_draw_surface(void*, const Surface*, SDL_Rect*, int, int, int, int, int, unsigned int, unsigned int) {}
static void null_move_target(void*, int, int) {}
static void null_render_prepare(void*, unsigned) {}
static void null_render_finish(void*) {}
static void null_render_area_prepare(void*, const SDL_Rect*) {}
static void null_render_area_finish(void*, Surface*) {}
static void null_capture_screen(void*, VideoScreenshotSignal) {}
static void null_signal_scene_change(void*) {}
static void null_signal_draw_atlas(void*, bool) {}
} // namespace detail

/** C video/renderers/null/null_renderer. Zero-output renderer. */
inline void null_renderer_set_callbacks(Renderer* r) {
    if (!r) return;
    r->is_available = detail::null_is_available;
    r->get_description = detail::null_get_description;
    r->get_name = detail::null_get_name;
    r->create = detail::null_create;
    r->destroy = detail::null_destroy;
    r->setup_context = detail::null_setup_context;
    r->get_context_state = detail::null_get_context_state;
    r->reset_context_with = detail::null_reset_context_with;
    r->reset_context = detail::null_reset_context;
    r->close_context = detail::null_close_context;
    r->draw_surface = detail::null_draw_surface;
    r->move_target = detail::null_move_target;
    r->render_prepare = detail::null_render_prepare;
    r->render_finish = detail::null_render_finish;
    r->render_area_prepare = detail::null_render_area_prepare;
    r->render_area_finish = detail::null_render_area_finish;
    r->capture_screen = detail::null_capture_screen;
    r->signal_scene_change = detail::null_signal_scene_change;
    r->signal_draw_atlas = detail::null_signal_draw_atlas;
    r->ctx = nullptr;
}

} // namespace video
} // namespace openomf

#endif // OPENOMF_VIDEO_NULLRENDERER_HPP
