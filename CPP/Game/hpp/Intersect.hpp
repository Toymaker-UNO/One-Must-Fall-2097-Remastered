#ifndef OPENOMF_GAME_INTERSECT_HPP
#define OPENOMF_GAME_INTERSECT_HPP

#include "Object.hpp"
#include "Utils/hpp/Vec.hpp"

namespace openomf {
namespace game {

/** C game/protos/intersect.h. 구현은 intersect.c 이식 시 추가. */
int intersect_object_object(Object* a, Object* b);
int intersect_object_point(Object* obj, openomf::utils::Vec2i point);
int intersect_sprite_hitpoint(Object* obj, Object* target, int level, openomf::utils::Vec2i* point);
int intersect_har_sprite_hitpoint(Object* obj, Object* target, int level, openomf::utils::Vec2i* point);

} // namespace game
} // namespace openomf

#endif // OPENOMF_GAME_INTERSECT_HPP
