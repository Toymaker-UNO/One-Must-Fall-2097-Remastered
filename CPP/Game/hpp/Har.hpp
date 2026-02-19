#ifndef OPENOMF_GAME_HAR_HPP
#define OPENOMF_GAME_HAR_HPP

#include <cstdint>

namespace openomf {
namespace game {

struct Object;

/** C har.h LAYER_* */
constexpr std::uint8_t LAYER_HAR = 0x02;
constexpr std::uint8_t LAYER_HAR1 = 0x04;
constexpr std::uint8_t LAYER_HAR2 = 0x08;
constexpr std::uint8_t LAYER_SCRAP = 0x10;
constexpr std::uint8_t LAYER_PROJECTILE = 0x20;
constexpr std::uint8_t LAYER_HAZARD = 0x40;

constexpr int STUN_RECOVERY_CONSTANT = 256 * 18 / 250;
constexpr int STUN_RECOVERY_BLOCKING_CONSTANT = 256 * 27 / 250;
constexpr int HEIGHT_STANDING = 55;
constexpr int HEIGHT_CROUCHING = 30;

/** C har.h category enum */
enum class HarCategory : int {
    CAT_MISC = 0,
    CAT_CLOSE = 2,
    CAT_LOW = 4,
    CAT_MEDIUM = 5,
    CAT_HIGH,
    CAT_JUMPING,
    CAT_PROJECTILE,
    CAT_BASIC,
    CAT_VICTORY,
    CAT_FIRE_ICE,
    CAT_SCRAP,
    CAT_DESTRUCTION,
};

/** C har.h state enum */
enum class HarState : std::uint8_t {
    STATE_NONE = 0,
    STATE_STANDING = 1,
    STATE_WALKTO,
    STATE_WALKFROM,
    STATE_CROUCHING,
    STATE_CROUCHBLOCK,
    STATE_JUMPING,
    STATE_RECOIL,
    STATE_STANDING_UP,
    STATE_STUNNED,
    STATE_BLOCKSTUN,
    STATE_VICTORY,
    STATE_DEFEAT,
    STATE_SCRAP,
    STATE_DESTRUCTION,
    STATE_WALLDAMAGE,
    STATE_DONE,
};

/** C har.h HAR_EVENT_* */
enum class HarEventType : std::uint8_t {
    HAR_EVENT_JUMP,
    HAR_EVENT_AIR_TURN,
    HAR_EVENT_WALK,
    HAR_EVENT_AIR_ATTACK_DONE,
    HAR_EVENT_ATTACK,
    HAR_EVENT_ENEMY_BLOCK,
    HAR_EVENT_ENEMY_BLOCK_PROJECTILE,
    HAR_EVENT_BLOCK,
    HAR_EVENT_BLOCK_PROJECTILE,
    HAR_EVENT_LAND_HIT,
    HAR_EVENT_LAND_HIT_PROJECTILE,
    HAR_EVENT_TAKE_HIT,
    HAR_EVENT_TAKE_HIT_PROJECTILE,
    HAR_EVENT_HAZARD_HIT,
    HAR_EVENT_ENEMY_HAZARD_HIT,
    HAR_EVENT_STUN,
    HAR_EVENT_ENEMY_STUN,
    HAR_EVENT_RECOVER,
    HAR_EVENT_HIT_WALL,
    HAR_EVENT_LAND,
    HAR_EVENT_DEFEAT,
    HAR_EVENT_SCRAP,
    HAR_EVENT_DESTRUCTION,
    HAR_EVENT_DONE,
};

/** C har_t forward decl. */
struct Har {};

} // namespace game
} // namespace openomf

#endif // OPENOMF_GAME_HAR_HPP
