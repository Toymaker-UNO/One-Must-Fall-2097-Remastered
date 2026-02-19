#ifndef OPENOMF_GAME_SCENEAPI_HPP
#define OPENOMF_GAME_SCENEAPI_HPP

#include "Scene.hpp"
#include "CommonDefines.hpp"
#include "TickTimerApi.hpp"
#include "Resources/hpp/PathManager.hpp"
#include "Resources/hpp/BkLoader.hpp"
#include "Formats/hpp/Bk.hpp"
#include "Video/hpp/Video.hpp"
#include "Video/hpp/Surface.hpp"
#include "Utils/hpp/Allocator.hpp"
#include <cstring>

namespace openomf {
namespace game {

/** C scene_create 대체: 리소스 로드 없이 씬만 초기화. */
inline int scene_create_empty(Scene* sc, GameState* gs, int scene_id) {
    if (!sc || !gs) return 1;
    if (scene_id == SCENE_NONE) return 1;
    std::memset(sc, 0, sizeof(Scene));
    sc->id = scene_id;
    sc->gs = gs;
    sc->static_ticks_since_start = 0;
    sc->bk_data = nullptr;
    sc->af_data[0] = nullptr;
    sc->af_data[1] = nullptr;
    ticktimer_init(&sc->tick_timer);
    return 0;
}

/** C scene_create: PathManager + BkLoader로 BK(또는 PCX) 로드, 배경 Surface 캐시 생성. */
inline int scene_create_with_resources(Scene* sc, GameState* gs, int scene_id) {
    if (!sc || !gs || scene_id == SCENE_NONE) return 1;
    std::memset(sc, 0, sizeof(Scene));
    sc->gs = gs;
    sc->id = scene_id;
    sc->static_ticks_since_start = 0;
    ticktimer_init(&sc->tick_timer);

    const char* path = nullptr;
    if (scene_id == static_cast<int>(SCENE_SCOREBOARD))
        path = resources::PathManager::get_resource_path(static_cast<unsigned int>(resources::ResourceId::BK_MENU));
    else if (scene_id == static_cast<int>(SCENE_LOBBY))
        path = resources::PathManager::get_resource_path(static_cast<unsigned int>(resources::ResourceId::PCX_NETARENA));
    else if (scene_id >= 1 && scene_id <= 16)
        path = resources::PathManager::get_resource_path(static_cast<unsigned int>(scene_id - 1));
    else
        path = resources::PathManager::get_resource_path(static_cast<unsigned int>(resources::ResourceId::BK_MENU));

    if (!path) return 1;

    sc->bk_data = static_cast<formats::BkFile*>(omf_calloc(1, sizeof(formats::BkFile)));
    formats::SdError err;
    if (scene_id == static_cast<int>(SCENE_LOBBY))
        err = formats::bk_load_from_pcx(sc->bk_data, path);
    else
        err = resources::load_bk_file(path, sc->bk_data);

    if (err != formats::SdError::SUCCESS) {
        formats::bk_free(sc->bk_data);
        omf_free(sc->bk_data);
        sc->bk_data = nullptr;
        return 1;
    }

    formats::VgaImage* bg = formats::bk_get_background(sc->bk_data);
    if (bg) {
        video::Surface* sur = static_cast<video::Surface*>(omf_calloc(1, sizeof(video::Surface)));
        video::surface_create_from_vga(sur, bg);
        sc->background_surface = sur;
    }
    return 0;
}

/** C scene_init: 틱타이머만 초기화. (bootstrap 애니메이션은 추후.) */
inline void scene_init(Scene* sc) {
    if (!sc) return;
    ticktimer_init(&sc->tick_timer);
}

/** C scene_free: 배경 Surface·bk_data 해제 후 free 콜백·ticktimer_close. */
inline void scene_free(Scene* sc) {
    if (!sc) return;
    if (sc->free_cb) sc->free_cb(sc);
    if (sc->background_surface) {
        video::surface_free(static_cast<video::Surface*>(sc->background_surface));
        omf_free(sc->background_surface);
        sc->background_surface = nullptr;
    }
    if (sc->bk_data) {
        formats::bk_free(sc->bk_data);
        omf_free(sc->bk_data);
        sc->bk_data = nullptr;
    }
    ticktimer_close(&sc->tick_timer);
}

inline void scene_set_userdata(Scene* sc, void* userdata) {
    if (sc) sc->userdata = userdata;
}
inline void* scene_get_userdata(const Scene* sc) {
    return sc ? sc->userdata : nullptr;
}

/** C scene_startup. */
inline void scene_startup(Scene* sc, int id, int* m_load, int* m_repeat) {
    if (!sc || !m_load || !m_repeat) return;
    *m_load = 0;
    *m_repeat = 0;
    if (sc->startup_cb) sc->startup_cb(sc, id, m_load, m_repeat);
}

/** C scene_event: 0 = handled. */
inline int scene_event(Scene* sc, void* sdl_event) {
    if (!sc) return 1;
    if (sc->event_cb) return sc->event_cb(sc, sdl_event);
    return 1;
}

/** C scene_render: 배경 video_draw 후 render 콜백. */
inline void scene_render(Scene* sc) {
    if (!sc) return;
    if (sc->background_surface)
        video::video_draw(static_cast<const video::Surface*>(sc->background_surface), 0, 0);
    if (sc->render_cb) sc->render_cb(sc);
}

inline void scene_render_overlay(Scene* sc) {
    if (!sc) return;
    if (sc->render_overlay_cb) sc->render_overlay_cb(sc);
}

inline void scene_debug(Scene* sc) {
    if (!sc) return;
    if (sc->debug_cb) sc->debug_cb(sc);
}

inline void scene_dynamic_tick(Scene* sc, int paused) {
    if (!sc) return;
    if (sc->dynamic_tick_cb) sc->dynamic_tick_cb(sc, paused);
}

inline void scene_static_tick(Scene* sc, int paused) {
    if (!sc) return;
    sc->static_ticks_since_start++;
    if (sc->static_tick_cb) sc->static_tick_cb(sc, paused);
}

inline void scene_input_poll(Scene* sc) {
    if (!sc) return;
    if (sc->input_poll_cb) sc->input_poll_cb(sc);
}

/** C scene_is_arena. */
inline bool scene_is_arena(const Scene* sc) {
    if (!sc) return false;
    return sc->id >= SCENE_ARENA0 && sc->id <= SCENE_ARENA4;
}

} // namespace game
} // namespace openomf

#endif // OPENOMF_GAME_SCENEAPI_HPP
