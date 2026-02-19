#ifndef OPENOMF_GAME_OBJECT_HPP
#define OPENOMF_GAME_OBJECT_HPP

#include "GameState.hpp"
#include "PlayerState.hpp"
#include "Serial.hpp"
#include "Utils/hpp/Vec.hpp"
#include <cstdint>

namespace openomf {
namespace video {
struct Surface;
}
namespace formats {
struct Animation;
}
namespace game {

/** 비디오 팔레트 변환 콜백. C vga_palette_transform (damage_tracker*, vga_palette*, void*). */
using PaletteTransformFn = void (*)(void* damage, void* palette, void* userdata);

struct GameState;
struct Object;

using ObjectFreeCb = void (*)(Object* obj);
using ObjectActCb = int (*)(Object* obj, int action);
using ObjectTickCb = void (*)(Object* obj);
using ObjectMoveCb = void (*)(Object* obj);
using ObjectCollideCb = void (*)(Object* a, Object* b);
using ObjectFinishCb = void (*)(Object* obj);
using ObjectDebugCb = void (*)(Object* obj);
using ObjectCloneCb = int (*)(Object* src, Object* dst);
using ObjectCloneFreeCb = int (*)(Object* obj);

constexpr int OBJECT_DEFAULT_LAYER = 0x01;
constexpr int OBJECT_EVENT_BUFFER_SIZE = 16;

enum class ObjectFace : int8_t {
    LEFT = -1,
    NONE = 0,
    RIGHT = 1,
};

enum class ObjectOwner { EXTERNAL, OBJECT };

enum class PlayDirection { BACKWARDS, FORWARDS };

enum class ObjectGroup : int8_t {
    UNKNOWN = 0x1,
    HAR = 0x2,
    SCRAP = 0x4,
    PROJECTILE = 0x8,
    HAZARD = 0x10,
    ANNOUNCEMENT = 0x20,
};

enum class ObjectEffect : std::uint32_t {
    NONE = 0,
    SHADOW = 0x1,
    DARK_TINT = 0x2,
    POSITIONAL_LIGHTING = 0x4,
    STASIS = 0x8,
    SATURATE = 0x10,
    GLOW = 0x20,
    TRAIL = 0x40,
    ADD = 0x80,
    HAR_QUIRKS = 0x100,
};

enum class ObjectFlags : std::uint32_t {
    NONE = 0,
    NEXT_ANIM_ON_OWNER_HIT = 0x1,
    NEXT_ANIM_ON_ENEMY_HIT = 0x2,
    MC = 0x4,
};

/** C game/protos/object.h object_t. */
struct Object {
    std::uint32_t id = 0;
    GameState* gs = nullptr;

    openomf::utils::Vec2f start = {};
    openomf::utils::Vec2f pos = {};
    openomf::utils::Vec2f vel = {};
    openomf::utils::Vec2f cvel = {};
    float vertical_velocity_modifier = 0.f;
    float horizontal_velocity_modifier = 0.f;
    int8_t direction = 0;
    int8_t group = 0;
    bool wall_collision = false;
    bool hit_pixels_disabled = false;
    bool crossup_protection = false;
    bool should_hitpause = false;

    int8_t q_counter = 0;
    int8_t q_val = 0;
    int8_t can_hit = 0;

    int8_t orb_val = 0;

    float x_percent = 0.f;
    float y_percent = 0.f;
    float gravity = 0.f;

    std::uint32_t frame_video_effects = 0;
    std::uint32_t animation_video_effects = 0;
    std::uint32_t object_flags = 0;

    std::uint8_t layers = 0;
    std::uint8_t cur_animation_own = 0;

    formats::Animation* cur_animation = nullptr;
    int cur_sprite_id = 0;
    const char* sound_translation_table = nullptr;
    std::uint8_t sprite_override = 0;

    std::uint32_t attached_to_id = 0;

    std::uint8_t pal_offset = 0;
    std::uint8_t pal_limit = 0;
    std::uint8_t halt = 0;
    int16_t halt_ticks = 0;
    std::uint8_t stride = 0;
    std::uint8_t cast_shadow = 0;
    int o_shadow_correction = 0;

    video::Surface* cur_surface = nullptr;

    PlayerSpriteState sprite_state = {};
    PlayerAnimationState animation_state = {};
    PlayerSlideState slide_state = {};

    std::uint32_t age = 0;

    void* userdata = nullptr;
    ObjectFreeCb free_cb = nullptr;
    ObjectActCb act_cb = nullptr;
    ObjectTickCb static_tick_cb = nullptr;
    ObjectTickCb dynamic_tick_cb = nullptr;
    ObjectCollideCb collide_cb = nullptr;
    ObjectFinishCb finish_cb = nullptr;
    ObjectMoveCb move_cb = nullptr;
    PaletteTransformFn palette_transform = nullptr;
    ObjectDebugCb debug_cb = nullptr;
    ObjectCloneCb clone_cb = nullptr;
    ObjectCloneFreeCb clone_free_cb = nullptr;
};

} // namespace game
} // namespace openomf

#endif // OPENOMF_GAME_OBJECT_HPP
