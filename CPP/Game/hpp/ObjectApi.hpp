#ifndef OPENOMF_GAME_OBJECTAPI_HPP
#define OPENOMF_GAME_OBJECTAPI_HPP

#include "Object.hpp"
#include "PlayerApi.hpp"
#include "Utils/hpp/Vec.hpp"
#include <cstring>

namespace openomf {
namespace game {

namespace detail {
inline std::uint32_t& next_object_id() {
    static std::uint32_t id = 1;
    return id;
}
}

/** C object_create. */
inline void object_create(Object* obj, GameState* gs, openomf::utils::Vec2i pos, openomf::utils::Vec2f vel) {
    if (!obj || !gs) return;
    std::memset(obj, 0, sizeof(Object));
    obj->gs = gs;
    obj->id = detail::next_object_id()++;

    obj->pos = openomf::utils::vec2i_to_f(pos);
    obj->start = obj->pos;
    obj->vel = vel;
    obj->horizontal_velocity_modifier = 1.0f;
    obj->vertical_velocity_modifier = 1.0f;
    obj->direction = static_cast<int8_t>(ObjectFace::RIGHT);
    obj->y_percent = 1.0f;
    obj->x_percent = 1.0f;

    obj->layers = OBJECT_DEFAULT_LAYER;
    obj->group = static_cast<int8_t>(ObjectGroup::UNKNOWN);
    obj->gravity = 0.0f;
    obj->animation_video_effects = 0;
    obj->frame_video_effects = 0;
    obj->attached_to_id = 0;
    obj->cur_animation_own = static_cast<std::uint8_t>(ObjectOwner::EXTERNAL);
    obj->cur_animation = nullptr;
    obj->cur_sprite_id = -1;
    obj->sprite_override = 0;
    obj->sound_translation_table = nullptr;
    obj->cur_surface = nullptr;
    obj->pal_offset = 0;
    obj->pal_limit = 255;
    obj->halt = 0;
    obj->halt_ticks = 0;
    obj->stride = 1;
    obj->cast_shadow = 0;
    obj->age = 0;

    player_create(obj);
}

/** C object_create_static. */
inline void object_create_static(Object* obj, GameState* gs) {
    object_create(obj, gs, openomf::utils::vec2i_create(0, 0), openomf::utils::vec2f_create(0.f, 0.f));
}

/** C object_free: free 콜백 호출 후 player_free. */
inline void object_free(Object* obj) {
    if (!obj) return;
    if (obj->free_cb) obj->free_cb(obj);
    player_free(obj);
}

/** C object_set_pos. */
inline void object_set_pos(Object* obj, openomf::utils::Vec2i pos) {
    if (!obj) return;
    obj->pos.x = static_cast<float>(pos.x);
    obj->pos.y = static_cast<float>(pos.y);
}

/** C object_get_pos. */
inline openomf::utils::Vec2i object_get_pos(const Object* obj) {
    if (!obj) return openomf::utils::vec2i_create(0, 0);
    return openomf::utils::vec2f_to_i(obj->pos);
}

/** C object_set_vel / object_get_vel. */
inline void object_set_vel(Object* obj, openomf::utils::Vec2f vel) {
    if (!obj) return;
    obj->vel = vel;
}
inline openomf::utils::Vec2f object_get_vel(const Object* obj) {
    if (!obj) return openomf::utils::vec2f_create(0.f, 0.f);
    return obj->vel;
}

/** C object_set_direction / object_get_direction. */
inline void object_set_direction(Object* obj, int dir) {
    if (!obj) return;
    obj->direction = static_cast<int8_t>(dir);
}
inline int object_get_direction(const Object* obj) {
    return obj ? static_cast<int>(obj->direction) : 0;
}

/** C object_static_tick / object_dynamic_tick (최소: 콜백만 호출, age 증가). */
inline void object_static_tick(Object* obj) {
    if (!obj) return;
    if (obj->static_tick_cb) obj->static_tick_cb(obj);
}

inline void object_dynamic_tick(Object* obj) {
    if (!obj) return;
    obj->age++;
    if (obj->dynamic_tick_cb) obj->dynamic_tick_cb(obj);
}

/** C object_set_userdata / object_get_userdata. */
inline void object_set_userdata(Object* obj, void* ptr) { if (obj) obj->userdata = ptr; }
inline void* object_get_userdata(const Object* obj) { return obj ? obj->userdata : nullptr; }

} // namespace game
} // namespace openomf

#endif // OPENOMF_GAME_OBJECTAPI_HPP
