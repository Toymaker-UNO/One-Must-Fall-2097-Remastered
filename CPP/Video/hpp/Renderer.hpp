#ifndef OPENOMF_VIDEO_RENDERER_HPP
#define OPENOMF_VIDEO_RENDERER_HPP

#include "hpp/Surface.hpp"
#include <SDL2/SDL.h>

namespace openomf {
namespace video {

struct Renderer;

/** Asynchronous screenshot callback. */
using VideoScreenshotSignal = void (*)(const SDL_Rect* rect, unsigned char* data, bool flipped);

/** Metadata (no context required). */
using IsAvailableFn = bool (*)(void);
using GetDescriptionFn = const char* (*)(void);
using GetNameFn = const char* (*)(void);

/** Lifecycle. */
using InitRendererFn = void (*)(Renderer* r);
using CloseRendererFn = void (*)(Renderer* r);

/** Context. */
using SetupContextFn = bool (*)(void* ctx, int window_w, int window_h, bool fullscreen, bool vsync, int aspect, int framerate_limit);
using GetContextStateFn = void (*)(void* ctx, int* window_w, int* window_h, bool* fullscreen, bool* vsync, int* aspect);
using ResetContextWithFn = bool (*)(void* ctx, int window_w, int window_h, bool fullscreen, bool vsync, int aspect, int framerate_limit);
using ResetContextFn = void (*)(void* ctx);
using CloseContextFn = void (*)(void* ctx);

/** Drawing. */
using DrawSurfaceFn = void (*)(void* ctx, const Surface* src_surface, SDL_Rect* rect, int remap_offset, int remap_rounds,
                               int palette_offset, int palette_limit, int opacity, unsigned int flip_mode, unsigned int options);
using MoveTargetFn = void (*)(void* ctx, int x, int y);

/** Onscreen / offscreen. */
using RenderPrepareFn = void (*)(void* ctx, unsigned framebuffer_options);
using RenderFinishFn = void (*)(void* ctx);
using RenderAreaPrepareFn = void (*)(void* ctx, const SDL_Rect* area);
using RenderAreaFinishFn = void (*)(void* ctx, Surface* dst);

using CaptureScreenFn = void (*)(void* ctx, VideoScreenshotSignal screenshot_cb);
using SignalSceneChangeFn = void (*)(void* ctx);
using SignalDrawAtlasFn = void (*)(void* ctx, bool toggle);

/** C video/renderers/renderer.h renderer struct. */
struct Renderer {
    IsAvailableFn is_available = nullptr;
    GetDescriptionFn get_description = nullptr;
    GetNameFn get_name = nullptr;

    InitRendererFn create = nullptr;
    CloseRendererFn destroy = nullptr;

    SetupContextFn setup_context = nullptr;
    ResetContextWithFn reset_context_with = nullptr;
    ResetContextFn reset_context = nullptr;
    GetContextStateFn get_context_state = nullptr;
    CloseContextFn close_context = nullptr;

    DrawSurfaceFn draw_surface = nullptr;
    MoveTargetFn move_target = nullptr;

    RenderPrepareFn render_prepare = nullptr;
    RenderFinishFn render_finish = nullptr;
    RenderAreaPrepareFn render_area_prepare = nullptr;
    RenderAreaFinishFn render_area_finish = nullptr;

    CaptureScreenFn capture_screen = nullptr;
    SignalSceneChangeFn signal_scene_change = nullptr;
    SignalDrawAtlasFn signal_draw_atlas = nullptr;

    void* ctx = nullptr;
};

} // namespace video
} // namespace openomf

#endif // OPENOMF_VIDEO_RENDERER_HPP
