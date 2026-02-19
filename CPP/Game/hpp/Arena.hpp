#ifndef OPENOMF_GAME_ARENA_HPP
#define OPENOMF_GAME_ARENA_HPP

#include "Scene.hpp"
#include "GameState.hpp"
#include <cstddef>
#include <cstdint>

namespace openomf {
namespace game {

/** C arena.h ARENA_STATE_* */
enum ArenaState {
    ARENA_STATE_STARTING,
    ARENA_STATE_FIGHTING,
    ARENA_STATE_ENDING,
};

constexpr std::uint8_t wall_slam_tolerance_default = 7;
constexpr std::uint8_t wall_slam_tolerance_powerplant = 5;

/** C arena_create: 아레나 씬 생성. 구현은 scene/arena 이식 시 (현재 stub). */
inline int arena_create(Scene* scene) {
    (void)scene;
    return 0;
}

inline int arena_get_state(const Scene* scene) {
    (void)scene;
    return ARENA_STATE_STARTING;
}

inline void arena_set_state(Scene* scene, int state) {
    (void)scene;
    (void)state;
}

inline void arena_toggle_rein(Scene* scene) {
    (void)scene;
}

inline void maybe_install_har_hooks(Scene* scene) {
    (void)scene;
}

inline std::uint32_t arena_state_hash(const GameState* gs) {
    (void)gs;
    return 0;
}

inline void arena_state_dump(const GameState* gs, char* buf, size_t bufsize) {
    (void)gs;
    if (buf && bufsize) buf[0] = '\0';
}

inline void arena_reset(Scene* sc) {
    (void)sc;
}

inline int arena_is_over(const Scene* sc) {
    (void)sc;
    return 0;
}

inline int arena_get_wall_slam_tolerance(const GameState* gs) {
    (void)gs;
    return static_cast<int>(wall_slam_tolerance_default);
}

} // namespace game
} // namespace openomf

#endif // OPENOMF_GAME_ARENA_HPP
