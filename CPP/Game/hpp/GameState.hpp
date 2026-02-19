#ifndef OPENOMF_GAME_GAMESTATE_HPP
#define OPENOMF_GAME_GAMESTATE_HPP

#include <cstdint>

namespace openomf {
namespace game {

// C의 game_state_type.h 를 C++로 옮길 준비를 위한 스켈레톤.
// init_flags는 Engine 쪽 EngineInitFlags*를 void*로 저장.

struct Scene;
struct GamePlayer;
struct TickTimer;
struct Controller;
struct Random;
struct SdRecFile;

struct MatchSettings {
    std::uint8_t throw_range{};
    std::uint8_t hit_pause{};
    std::uint8_t block_damage{};
    std::uint8_t vitality{};
    std::uint8_t jump_height{};
    std::uint8_t knock_down{}; // knock_down_mode
    bool rehit{};
    bool defensive_throws{};
    std::uint8_t power1{};
    std::uint8_t power2{};
    bool hazards{};
    std::uint8_t rounds{};
    bool fight_mode{};
    bool sim{};
};

struct GameState {
    unsigned int run{};
    unsigned int paused{};
    unsigned int this_id{};
    unsigned int next_id{};
    unsigned int next_next_id{};
    std::uint32_t tick{};
    std::uint32_t int_tick{};
    unsigned int role{};
    unsigned int speed{};
    void* init_flags{};  /* engine::EngineInitFlags* */

    MatchSettings match_settings{};

    int screen_shake_horizontal{};
    int screen_shake_vertical{};

    int arena{};

    int speed_slowdown_previous{};
    int speed_slowdown_time{};

    int hit_pause{};

    int next_wait_ticks{};
    int this_wait_ticks{};

    bool hide_ui{};
    int warp_speed{};

    int net_mode{};
    Scene* sc{};
    /** C game_state menu_ctrl. 메뉴/입력용 컨트롤러 (controller::Controller*). */
    void* menu_ctrl{};
    /** C game_state new_state. 씬/컨트롤러에서 상태 교체 시 사용. */
    GameState* new_state{};
    /** C game_state delay. 틱 지연(예: hit_pause). */
    int delay{};
    // objects, sounds, players, fight_stats, rand, rec 등은 추후 이식.
};

} // namespace game
} // namespace openomf

#endif // OPENOMF_GAME_GAMESTATE_HPP

