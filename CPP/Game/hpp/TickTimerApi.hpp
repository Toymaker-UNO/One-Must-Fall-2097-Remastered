#ifndef OPENOMF_GAME_TICKTIMERAPI_HPP
#define OPENOMF_GAME_TICKTIMERAPI_HPP

#include "TickTimer.hpp"
#include "Utils/hpp/Vector.hpp"
#include <cstddef>

namespace openomf {
namespace game {

/** C ticktimer_cb */
using TickTimerCb = void (*)(void* scenedata, void* userdata);

namespace detail {
struct TickTimerUnit {
    TickTimerCb callback = nullptr;
    int ticks = 0;
    void* userdata = nullptr;
};
} // namespace detail

inline void ticktimer_init(TickTimer* tt) {
    if (!tt) return;
    tt->units.vector_create(sizeof(detail::TickTimerUnit));
}

inline void ticktimer_close(TickTimer* tt) {
    if (!tt) return;
    tt->units.vector_free();
}

inline void ticktimer_add(TickTimer* tt, int ticks, TickTimerCb cb, void* userdata) {
    if (!tt || !cb) return;
    detail::TickTimerUnit unit;
    unit.callback = cb;
    unit.ticks = ticks;
    unit.userdata = userdata;
    tt->units.vector_append(&unit);
}

inline void ticktimer_run(TickTimer* tt, void* scenedata) {
    if (!tt) return;
    unsigned int n = tt->units.vector_size();
    for (unsigned int i = 0; i < n; i++) {
        detail::TickTimerUnit* unit = static_cast<detail::TickTimerUnit*>(tt->units.vector_get(i));
        if (unit) unit->ticks--;
    }
    for (int i = static_cast<int>(n) - 1; i >= 0; i--) {
        detail::TickTimerUnit* unit = static_cast<detail::TickTimerUnit*>(tt->units.vector_get(static_cast<unsigned int>(i)));
        if (unit && unit->ticks <= 0) {
            unit->callback(scenedata, unit->userdata);
            tt->units.vector_swapdelete_at(static_cast<unsigned int>(i));
        }
    }
}

inline void ticktimer_clone(const TickTimer* src, TickTimer* dst) {
    if (!src || !dst) return;
    ticktimer_init(dst);
    unsigned int n = src->units.vector_size();
    for (unsigned int i = 0; i < n; i++) {
        const detail::TickTimerUnit* u = static_cast<const detail::TickTimerUnit*>(src->units.vector_get(i));
        if (u) dst->units.vector_append(u);
    }
}

} // namespace game
} // namespace openomf

#endif // OPENOMF_GAME_TICKTIMERAPI_HPP
