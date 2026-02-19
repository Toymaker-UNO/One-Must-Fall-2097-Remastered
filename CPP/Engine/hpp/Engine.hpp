#ifndef OPENOMF_ENGINE_ENGINE_HPP
#define OPENOMF_ENGINE_ENGINE_HPP

#include "Game/hpp/GameState.hpp"
#include "Game/hpp/GameStateApi.hpp"
#include "Game/hpp/SceneApi.hpp"
#include "Game/hpp/CommonDefines.hpp"
#include "Controller/hpp/KeyboardController.hpp"
#include "Video/hpp/Video.hpp"
#include "Audio/hpp/Audio.hpp"
#include "Console/hpp/Console.hpp"
#include "Utils/hpp/Allocator.hpp"
#include "ExternalLibrary/hpp/Sdl2.hpp"
#include <cstring>

/* Engine 빌드 시 -I .. -I ../Video -I ../Utils -I ../Formats -I ../Game -I ../Audio -I ../Console -I ../ExternalLibrary 필요 */

namespace openomf {
namespace engine {

/** C engine.h STATIC_TICKS (ms) */
constexpr int STATIC_TICKS_MS = 10;
/** C engine.c MAX_TICKS_PER_FRAME */
constexpr int MAX_TICKS_PER_FRAME = 10;
/** C engine.c TICK_EXPIRY_MS */
constexpr int TICK_EXPIRY_MS = 100;

/** C engine_init_flags_t. 옵션은 omf_config.json 또는 기본값으로 설정. */
struct EngineInitFlags {
    unsigned int net_mode = 0;
    unsigned int record = 0;
    unsigned int playback = 0;
    char force_renderer[16] = {};
    char force_audio_backend[16] = {};
    char rec_file[255] = {};
    int warpspeed = 0;
    int speed = -1;
};

/** C game_state_create 최소 대체: run=1, 초기 씬 설정 + 리소스 로드(씬 배경). */
inline int game_state_create_stub(game::GameState* gs, EngineInitFlags* init_flags) {
    if (!gs || !init_flags) return 1;
    std::memset(gs, 0, sizeof(game::GameState));
    gs->run = 1;
    gs->this_id = game::SCENE_MENU;
    gs->next_id = game::SCENE_NONE;
    gs->init_flags = init_flags;

    gs->sc = static_cast<game::Scene*>(omf_calloc(1, sizeof(game::Scene)));
    if (game::scene_create_with_resources(gs->sc, gs, static_cast<int>(game::SCENE_MENU)) != 0) {
        game::scene_free(gs->sc);
        omf_free(gs->sc);
        gs->sc = nullptr;
    } else {
        game::scene_init(gs->sc);
    }

    gs->menu_ctrl = omf_calloc(1, sizeof(controller::Controller));
    controller::keyboard_controller_init(static_cast<controller::Controller*>(gs->menu_ctrl));
    return 0;
}

/** C game_state_is_running */
inline unsigned int game_state_is_running(const game::GameState* gs) {
    return gs ? gs->run : 0;
}

/** C game_state_free: menu_ctrl·씬 해제 후 GameState 해제. */
inline void game_state_free_stub(game::GameState** gs) {
    if (!gs || !*gs) return;
    if ((*gs)->menu_ctrl) {
        controller::controller_free(static_cast<controller::Controller*>((*gs)->menu_ctrl));
        omf_free((*gs)->menu_ctrl);
        (*gs)->menu_ctrl = nullptr;
    }
    if ((*gs)->sc) {
        game::scene_free((*gs)->sc);
        omf_free((*gs)->sc);
        (*gs)->sc = nullptr;
    }
    omf_free(*gs);
    *gs = nullptr;
}

/** C engine_init: video, audio, console 초기화. (sounds_loader, lang, fonts, altpals, vga_state는 추후.) */
inline int engine_init(EngineInitFlags* init_flags) {
    if (!init_flags) return 1;
    const char* renderer = init_flags->force_renderer[0] ? init_flags->force_renderer : nullptr;
    const char* audio_backend = init_flags->force_audio_backend[0] ? init_flags->force_audio_backend : nullptr;
    int w = 320, h = 200;
    video::video_scan_renderers();
    if (!video::video_find_renderer(renderer)) {}
    if (!video::video_init(renderer, w, h, false, false, 0, 0)) return 1;
    audio::audio_scan_backends();
    if (!audio::audio_find_backend(audio_backend)) {}
    if (!audio::audio_init(audio_backend, 44100, false, 0, 0.8f, 0.8f)) {
        video::video_close();
        return 1;
    }
    if (!console::console_init()) {
        audio::audio_close();
        video::video_close();
        return 1;
    }
    return 0;
}

/** C engine_close */
inline void engine_close() {
    console::console_close();
    audio::audio_close();
    video::video_close();
}

/** C engine_run: 이벤트 루프·씬 틱·렌더·리소스 연동. */
inline void engine_run(EngineInitFlags* init_flags) {
    if (!init_flags) return;
    SDL_Event e;
    int enable_screen_updates = 1;

    game::GameState* gs = static_cast<game::GameState*>(omf_calloc(1, sizeof(game::GameState)));
    if (game_state_create_stub(gs, init_flags) != 0) {
        game_state_free_stub(&gs);
        return;
    }

    int run = 1;
    Uint64 frame_start = SDL_GetTicks64();
    int dynamic_wait = 0;
    int static_wait = 0;

    while (run && game_state_is_running(gs)) {
        /* 이벤트 처리 */
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_QUIT:
                    run = 0;
                    break;
                case SDL_KEYDOWN: {
                    SDL_Keycode sym = e.key.keysym.sym;
                    SDL_Scancode sc = e.key.keysym.scancode;
                    if (console::console_window_is_open() &&
                        (sc == SDL_SCANCODE_GRAVE || sym == SDLK_BACKQUOTE || sym == SDLK_TAB || sym == SDLK_ESCAPE)) {
                        console::console_window_close();
                        continue;
                    }
                    if (sym == SDLK_TAB || sym == SDLK_BACKQUOTE || sc == SDL_SCANCODE_GRAVE) {
                        console::console_window_open();
                        continue;
                    }
                    break;
                }
                case SDL_WINDOWEVENT:
                    if (e.window.event == SDL_WINDOWEVENT_MINIMIZED || e.window.event == SDL_WINDOWEVENT_HIDDEN)
                        enable_screen_updates = 0;
                    else if (e.window.event == SDL_WINDOWEVENT_MAXIMIZED ||
                             e.window.event == SDL_WINDOWEVENT_RESTORED ||
                             e.window.event == SDL_WINDOWEVENT_SHOWN)
                        enable_screen_updates = 1;
                    break;
                default:
                    break;
            }
            if (console::console_window_is_open())
                console::console_event(gs, &e);
            else
                game::game_state_handle_event(gs, &e);
        }

        Uint64 now = SDL_GetTicks64();
        Uint64 frame_dt = now - frame_start;
        frame_start = now;
        dynamic_wait += static_cast<int>(frame_dt);
        static_wait += static_cast<int>(frame_dt);
        if (dynamic_wait > TICK_EXPIRY_MS) dynamic_wait = TICK_EXPIRY_MS;
        if (static_wait > TICK_EXPIRY_MS) static_wait = TICK_EXPIRY_MS;

        int tick_limit = MAX_TICKS_PER_FRAME;
        int dyntick_ms = game::game_state_ms_per_dyntick(gs);
        bool has_static;
        bool has_dynamic;
        do {
            has_static = static_wait > STATIC_TICKS_MS;
            if (has_static) {
                game::game_state_static_tick(gs, false);
                if (gs->new_state) {
                    game::GameState* old_gs = gs;
                    gs = gs->new_state;
                    game::game_state_clone_free(old_gs);
                    omf_free(old_gs);
                }
                static_wait -= STATIC_TICKS_MS;
            }
            has_dynamic = dynamic_wait > dyntick_ms;
            if (has_dynamic) {
                game::game_state_dynamic_tick(gs, false);
                dynamic_wait -= dyntick_ms;
                if (gs->delay > 0) {
                    SDL_Delay(4);
                    gs->delay--;
                    dynamic_wait -= 4;
                }
            }
            if (has_dynamic || has_static)
                game::game_state_palette_transform(gs);
        } while (tick_limit-- && (has_dynamic || has_static));

        if (enable_screen_updates) {
            video::video_render_prepare(game::game_state_get_framebuffer_options(gs));
            game::game_state_render(gs);
            console::console_render();
            video::video_render_finish();
        } else {
            SDL_Delay(1);
        }
    }

    game_state_free_stub(&gs);
}

} // namespace engine
} // namespace openomf

#endif // OPENOMF_ENGINE_ENGINE_HPP
