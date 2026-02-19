#ifndef OPENOMF_GAME_GAMESTATEAPI_HPP
#define OPENOMF_GAME_GAMESTATEAPI_HPP

#include "GameState.hpp"
#include "CommonDefines.hpp"
#include "SceneApi.hpp"
#include "Console/hpp/Console.hpp"
#include "Controller/hpp/Controller.hpp"
#include "ExternalLibrary/hpp/Sdl2.hpp"

namespace openomf {
namespace game {

/** C game_state_get_player. 1차 이식: 플레이어 배열 미구현 시 nullptr. */
inline void* game_state_get_player(const GameState* gs, int /*player_id*/) {
    (void)gs;
    return nullptr;
}

/** C game_state_handle_event. 1차 이식: 씬/오브젝트 이벤트 미연동 시 1 반환(미처리). */
inline int game_state_handle_event(GameState* gs, SDL_Event* /*event*/) {
    (void)gs;
    return 1;
}

/** C game_state_render: scene_render + scene_render_overlay. (오브젝트 레이어는 추후.) */
inline void game_state_render(GameState* gs) {
    if (!gs) return;
    if (gs->sc) {
        scene_render(gs->sc);
        scene_render_overlay(gs->sc);
    }
}

/** C game_state_get_framebuffer_options. 1차 이식: 0. (FBUFOPT_CREDITS 등 추후.) */
inline unsigned game_state_get_framebuffer_options(const GameState* /*gs*/) {
    return 0;
}

/** C game_state_ms_per_dyntick. 1차 이식: 고정 17ms (약 60fps). */
inline int game_state_ms_per_dyntick(const GameState* /*gs*/) {
    return 17;
}

/** C game_state_palette_transform. 1차 이식: no-op. (vga_state 연동 추후.) */
inline void game_state_palette_transform(GameState* /*gs*/) {
}

/** C game_state_debug. 1차 이식: no-op. */
inline void game_state_debug(GameState* /*gs*/) {
}

/** C game_state_tick_controllers. menu_ctrl 틱. */
inline void game_state_tick_controllers(GameState* gs) {
    if (!gs || !gs->menu_ctrl) return;
    controller::controller_tick(static_cast<controller::Controller*>(gs->menu_ctrl), gs->tick, nullptr);
}

/** C game_state_static_tick. 대기 틱 감소, 씬 전환(SCENE_NONE 시 run=0), tick_controllers, console_tick. */
inline void game_state_static_tick(GameState* gs, bool /*replay*/) {
    if (!gs) return;
    if (gs->this_wait_ticks > 0) gs->this_wait_ticks--;
    if (gs->next_wait_ticks > 0) gs->next_wait_ticks--;
    if (gs->this_id != gs->next_id) {
        if (gs->next_id == static_cast<unsigned int>(SCENE_NONE)) {
            gs->run = 0;
            return;
        }
        // game_load_new 등 풀 이식 전에는 씬 전환 no-op (run 유지)
    }
    game_state_tick_controllers(gs);
    console::console_tick(gs);
}

/** C game_state_dynamic_tick. 1차 이식: hit_pause/delay 감소만. */
inline void game_state_dynamic_tick(GameState* gs, bool /*replay*/) {
    if (!gs) return;
    if (gs->hit_pause > 0) {
        gs->hit_pause--;
        gs->int_tick++;
        return;
    }
    if (gs->delay > 0) gs->delay--;
}

/** C game_state_clone_free. 1차 이식: 별도 할당 없이 clone 사용 안 함이면 no-op. */
inline void game_state_clone_free(GameState* /*gs*/) {
}

} // namespace game
} // namespace openomf

#endif // OPENOMF_GAME_GAMESTATEAPI_HPP
