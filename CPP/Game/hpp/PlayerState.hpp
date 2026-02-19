#ifndef OPENOMF_GAME_PLAYERSTATE_HPP
#define OPENOMF_GAME_PLAYERSTATE_HPP

#include "Utils/hpp/Vec.hpp"
#include <cstdint>

namespace openomf {
namespace game {

struct Object;

/** C formats/script.h sd_script 전방 선언. (Formats에 Script 이식 전까지 불투명) */
struct SdScript;

/** C game/protos/player.h object_state_add_cb. */
using ObjectStateAddCb = void (*)(Object* parent, int id, openomf::utils::Vec2i pos, openomf::utils::Vec2f vel,
                                   std::uint8_t mp_flags, int s, int g, void* userdata);
/** C object_state_del_cb. */
using ObjectStateDelCb = void (*)(Object* parent, int id, void* userdata);
/** C object_state_disable_cb. */
using ObjectStateDisableCb = void (*)(Object* parent, std::uint8_t id, std::uint16_t duration, void* userdata);

/** C player_sprite_state_t. */
struct PlayerSpriteState {
    int flipmode = 0;
    int timer = 0;
    int duration = 0;
    int screen_shake_horizontal = 0;
    int screen_shake_vertical = 0;

    openomf::utils::Vec2i o_correction = {};
    int disable_gravity = 0;

    int blend_start = 0;
    int blend_finish = 0;

    int pal_ref_index = 0;
    int pal_entry_count = 0;
    int pal_start_index = 0;
    int pal_begin = 0;
    int pal_end = 0;
    int pal_tint = 0;

    bool pal_tricks_off = false;
    bool bd_flag = false;
};

/** C player_slide_op_t (player_slide_state). */
struct PlayerSlideState {
    openomf::utils::Vec2f vel = {};
    int timer = 0;
};

/** C player_animation_state_t. */
struct PlayerAnimationState {
    std::uint32_t previous_tick = 0;
    std::uint32_t current_tick = 0;
    int previous = 0;
    int entered_frame = 0;
    SdScript* parser = nullptr;  // C: sd_script
    std::uint8_t repeat = 0;
    std::uint8_t reverse = 0;
    std::uint8_t finished = 0;
    std::uint8_t disable_d = 0;
    std::uint8_t shadow_corner_hack = 0;
    bool looping = false;

    std::uint8_t pal_copy_entries = 0;
    std::uint8_t pal_copy_start = 0;
    std::uint8_t pal_copy_count = 0;

    void* spawn_userdata = nullptr;
    void* destroy_userdata = nullptr;
    void* disable_userdata = nullptr;
    std::uint32_t enemy_obj_id = 0;
    ObjectStateAddCb spawn = nullptr;
    ObjectStateDelCb destroy = nullptr;
    ObjectStateDisableCb disable = nullptr;
};

} // namespace game
} // namespace openomf

#endif // OPENOMF_GAME_PLAYERSTATE_HPP
