#ifndef OPENOMF_GAME_PLAYERAPI_HPP
#define OPENOMF_GAME_PLAYERAPI_HPP

#include "Object.hpp"
#include <cstring>

namespace openomf {
namespace game {

/** C player_create: 애니메이션/스프라이트 상태 초기화. SdScript 미이식이므로 parser는 nullptr 유지. */
inline void player_create(Object* obj) {
    if (!obj) return;
    std::memset(&obj->animation_state, 0, sizeof(PlayerAnimationState));
    obj->animation_state.previous_tick = ~0u;
    std::memset(&obj->sprite_state, 0, sizeof(PlayerSpriteState));
    std::memset(&obj->slide_state, 0, sizeof(PlayerSlideState));
}

/** C player_free: parser 해제. SdScript 미이식이므로 no-op. */
inline void player_free(Object* obj) {
    (void)obj;
}

/** C player_reset: 틱/완료 플래그 초기화. */
inline void player_reset(Object* obj) {
    if (!obj) return;
    obj->animation_state.previous_tick = ~0u;
    obj->animation_state.current_tick = 0;
    obj->animation_state.finished = 0;
    obj->animation_state.previous = -1;
    obj->animation_state.disable_d = 0;
}

} // namespace game
} // namespace openomf

#endif // OPENOMF_GAME_PLAYERAPI_HPP
