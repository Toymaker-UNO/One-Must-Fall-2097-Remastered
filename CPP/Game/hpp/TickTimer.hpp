#ifndef OPENOMF_GAME_TICKTIMER_HPP
#define OPENOMF_GAME_TICKTIMER_HPP

#include "Utils/hpp/Vector.hpp"

namespace openomf {
namespace game {

/** C game/utils/ticktimer.h ticktimer_t. units는 콜백 유닛 목록. */
struct TickTimer {
    openomf::utils::Vector units;
};

} // namespace game
} // namespace openomf

#endif // OPENOMF_GAME_TICKTIMER_HPP
