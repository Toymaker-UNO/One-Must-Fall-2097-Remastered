#ifndef OPENOMF_GAME_SCENE_HPP
#define OPENOMF_GAME_SCENE_HPP

#include "GameState.hpp"
#include "Object.hpp"
#include "TickTimer.hpp"
#include <cstdint>

namespace openomf {
namespace formats {
struct BkFile;
struct AfFile;
}
namespace game {

struct GameState;
struct Object;
struct Scene;

/** C scene_free_cb 등. SDL_Event는 전방 선언으로 대체. */
struct SdlEvent;
using SceneFreeCb = void (*)(Scene* scene);
using SceneEventCb = int (*)(Scene* scene, void* sdl_event);
using SceneRenderCb = void (*)(Scene* scene);
using SceneRenderOverlayCb = void (*)(Scene* scene);
using SceneDebugCb = void (*)(Scene* scene);
using SceneTickCb = void (*)(Scene* scene, int paused);
using SceneInputPollCb = void (*)(Scene* scene);
using SceneStartupCb = void (*)(Scene* scene, int anim_id, int* m_load, int* m_repeat);
using SceneAnimPrioOverrideCb = int (*)(Scene* scene, int anim_id);
using SceneCloneCb = void (*)(Scene* src, Scene* dst);
using SceneCloneFreeCb = void (*)(Scene* scene);

/** C game/protos/scene.h scene_t. background_surface는 video::Surface* (캐시, SceneApi에서 생성/해제). */
struct Scene {
    GameState* gs = nullptr;
    int id = 0;
    formats::BkFile* bk_data = nullptr;
    formats::AfFile* af_data[2] = { nullptr, nullptr };
    void* background_surface = nullptr;  ///< video::Surface* from bk_data->background
    void* userdata = nullptr;
    int static_ticks_since_start = 0;

    SceneFreeCb free_cb = nullptr;
    SceneEventCb event_cb = nullptr;
    SceneRenderCb render_cb = nullptr;
    SceneRenderOverlayCb render_overlay_cb = nullptr;
    SceneDebugCb debug_cb = nullptr;
    SceneTickCb static_tick_cb = nullptr;
    SceneTickCb dynamic_tick_cb = nullptr;
    SceneInputPollCb input_poll_cb = nullptr;
    SceneStartupCb startup_cb = nullptr;
    SceneAnimPrioOverrideCb prio_override_cb = nullptr;
    SceneCloneCb clone_cb = nullptr;
    SceneCloneFreeCb clone_free_cb = nullptr;
    TickTimer tick_timer = {};
};

} // namespace game
} // namespace openomf

#endif // OPENOMF_GAME_SCENE_HPP
