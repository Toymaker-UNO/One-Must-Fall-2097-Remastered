#ifndef OPENOMF_CONTROLLER_KEYBOARDCONTROLLER_HPP
#define OPENOMF_CONTROLLER_KEYBOARDCONTROLLER_HPP

#include "Controller.hpp"
#include "ExternalLibrary/hpp/Sdl2.hpp"
#include <SDL2/SDL.h>
#include <cstring>

namespace openomf {
namespace controller {

namespace detail {
inline int keyboard_poll(Controller* ctrl, CtrlEvent** /*ev*/) {
    if (!ctrl) return 0;
    const Uint8* keys = SDL_GetKeyboardState(nullptr);
    if (!keys) {
        ctrl->current = ACT_NONE;
        ctrl->last = ctrl->current;
        return 0;
    }
    int a = ACT_NONE;
    if (keys[SDL_SCANCODE_UP])    a |= ACT_UP;
    if (keys[SDL_SCANCODE_DOWN])  a |= ACT_DOWN;
    if (keys[SDL_SCANCODE_LEFT])  a |= ACT_LEFT;
    if (keys[SDL_SCANCODE_RIGHT]) a |= ACT_RIGHT;
    if (keys[SDL_SCANCODE_ESCAPE]) a |= ACT_ESC;
    if (keys[SDL_SCANCODE_SPACE])  a |= ACT_PUNCH;
    if (keys[SDL_SCANCODE_RETURN] || keys[SDL_SCANCODE_RETURN2]) a |= ACT_KICK;
    ctrl->current = a;
    ctrl->last = ctrl->current;
    return 0;
}
inline int keyboard_tick(Controller* ctrl, std::uint32_t /*ticks*/, CtrlEvent** ev) {
    return keyboard_poll(ctrl, ev);
}
inline int keyboard_dyntick(Controller* ctrl, std::uint32_t /*ticks*/, CtrlEvent** ev) {
    return keyboard_poll(ctrl, ev);
}
inline void keyboard_free(Controller* /*ctrl*/) {}
} // namespace detail

/** C keyboard.c 대응. SDL 키보드 상태를 ACT_* 플래그로 변환. */
inline void keyboard_controller_init(Controller* ctrl) {
    if (!ctrl) return;
    controller_init(ctrl);
    ctrl->type = 1;  /* CTRL_TYPE_KEYBOARD */
    ctrl->tick_fun = detail::keyboard_tick;
    ctrl->dyntick_fun = detail::keyboard_dyntick;
    ctrl->poll_fun = detail::keyboard_poll;
    ctrl->rumble_fun = nullptr;
    ctrl->rewind_fun = nullptr;
    ctrl->free_fun = detail::keyboard_free;
    ctrl->data = nullptr;
}

} // namespace controller
} // namespace openomf

#endif // OPENOMF_CONTROLLER_KEYBOARDCONTROLLER_HPP
