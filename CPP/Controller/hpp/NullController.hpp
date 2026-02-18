#ifndef OPENOMF_CONTROLLER_NULLCONTROLLER_HPP
#define OPENOMF_CONTROLLER_NULLCONTROLLER_HPP

#include "Controller.hpp"

namespace openomf {
namespace controller {

namespace detail {
inline int null_tick(Controller*, std::uint32_t, CtrlEvent**) { return 0; }
inline int null_dyntick(Controller*, std::uint32_t, CtrlEvent**) { return 0; }
inline int null_poll(Controller*, CtrlEvent**) { return 0; }
inline int null_rumble(Controller*, float, int) { return 0; }
inline void null_rewind(Controller*) {}
inline void null_free(Controller*) {}
} // namespace detail

/** 입력/출력이 전혀 없는 Null 컨트롤러 설정. */
inline void null_controller_init(Controller* ctrl) {
    if (!ctrl) return;
    controller_init(ctrl);
    ctrl->type = 0;
    ctrl->tick_fun = detail::null_tick;
    ctrl->dyntick_fun = detail::null_dyntick;
    ctrl->poll_fun = detail::null_poll;
    ctrl->rumble_fun = detail::null_rumble;
    ctrl->rewind_fun = detail::null_rewind;
    ctrl->free_fun = detail::null_free;
}

} // namespace controller
} // namespace openomf

#endif // OPENOMF_CONTROLLER_NULLCONTROLLER_HPP

