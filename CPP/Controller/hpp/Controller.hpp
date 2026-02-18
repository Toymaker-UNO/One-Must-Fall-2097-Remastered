#ifndef OPENOMF_CONTROLLER_CONTROLLER_HPP
#define OPENOMF_CONTROLLER_CONTROLLER_HPP

#include <cstdint>
#include <cstring>

namespace openomf {
namespace controller {

/** C controller.h ACT_* 플래그 대응. */
enum ActionFlags : int {
    ACT_NONE  = 0x00,
    ACT_STOP  = 0x01,
    ACT_KICK  = 0x02,
    ACT_PUNCH = 0x04,
    ACT_UP    = 0x08,
    ACT_DOWN  = 0x10,
    ACT_LEFT  = 0x20,
    ACT_RIGHT = 0x40,
    ACT_ESC   = 0x80,

    ACT_Mask_Dirs = (ACT_UP | ACT_DOWN | ACT_LEFT | ACT_RIGHT),
};

/** C controller.h EVENT_TYPE_* 중 ACTION/CLOSE 만 사용. */
enum EventType : int {
    EVENT_TYPE_ACTION = 0,
    EVENT_TYPE_CLOSE  = 1,
};

struct CtrlEvent {
    int type = EVENT_TYPE_ACTION;
    int action = 0;
    CtrlEvent* next = nullptr;
};

/** C controller_t 의 축약 버전 (game_state, hooks, delay 버퍼 등은 추후 이식). */
struct Controller {
    int current = 0;
    int last = 0;
    int queued = 0;

    std::uint8_t delay = 0;
    bool supports_delay = false;
    int repeat = 0;
    int repeat_tick = 0;
    int rtt = 0;

    int type = 0;

    int (*tick_fun)(Controller* ctrl, std::uint32_t ticks, CtrlEvent** ev) = nullptr;
    int (*dyntick_fun)(Controller* ctrl, std::uint32_t ticks, CtrlEvent** ev) = nullptr;
    int (*poll_fun)(Controller* ctrl, CtrlEvent** ev) = nullptr;
    int (*rumble_fun)(Controller* ctrl, float magnitude, int duration) = nullptr;
    void (*rewind_fun)(Controller* ctrl) = nullptr;
    void (*controller_hook)(Controller* ctrl, int action) = nullptr;
    void (*free_fun)(Controller* ctrl) = nullptr;

    void* data = nullptr;
};

inline void controller_init(Controller* ctrl) {
    if (!ctrl) return;
    std::memset(ctrl, 0, sizeof(Controller));
}

inline void controller_cmd(Controller* ctrl, int action, CtrlEvent** ev) {
    if (!ev) return;
    if (ctrl) {
        ctrl->current |= action;
        ctrl->last = ctrl->current;
    }
    if (action == ACT_NONE) {
        action = ACT_STOP;
    }
    CtrlEvent* node = new CtrlEvent{};
    node->type = EVENT_TYPE_ACTION;
    node->action = action;
    node->next = nullptr;

    if (*ev == nullptr) {
        *ev = node;
    } else {
        CtrlEvent* it = *ev;
        while (it->next) it = it->next;
        it->next = node;
    }
}

inline void controller_close(Controller* /*ctrl*/, CtrlEvent** ev) {
    if (!ev) return;
    CtrlEvent* node = new CtrlEvent{};
    node->type = EVENT_TYPE_CLOSE;
    node->action = 0;
    node->next = nullptr;
    if (*ev == nullptr) {
        *ev = node;
    } else {
        CtrlEvent* it = *ev;
        while (it->next) it = it->next;
        it->next = node;
    }
}

inline int controller_poll(Controller* ctrl, CtrlEvent** ev) {
    if (!ctrl || !ctrl->poll_fun) return 0;
    return ctrl->poll_fun(ctrl, ev);
}

inline int controller_tick(Controller* ctrl, std::uint32_t ticks, CtrlEvent** ev) {
    if (!ctrl) return 0;
    if (ctrl->tick_fun) return ctrl->tick_fun(ctrl, ticks, ev);
    if (ctrl->poll_fun) return ctrl->poll_fun(ctrl, ev);
    return 0;
}

inline int controller_dyntick(Controller* ctrl, std::uint32_t ticks, CtrlEvent** ev) {
    if (!ctrl) return 0;
    if (ctrl->dyntick_fun) return ctrl->dyntick_fun(ctrl, ticks, ev);
    if (ctrl->poll_fun) return ctrl->poll_fun(ctrl, ev);
    return 0;
}

inline void controller_rewind(Controller* ctrl) {
    if (ctrl && ctrl->rewind_fun) ctrl->rewind_fun(ctrl);
}

inline int controller_rumble(Controller* ctrl, float magnitude, int duration) {
    if (!ctrl || !ctrl->rumble_fun) return 0;
    return ctrl->rumble_fun(ctrl, magnitude, duration);
}

inline void controller_free_chain(CtrlEvent* ev) {
    CtrlEvent* cur = ev;
    while (cur) {
        CtrlEvent* next = cur->next;
        delete cur;
        cur = next;
    }
}

inline void controller_free(Controller* ctrl) {
    if (!ctrl) return;
    if (ctrl->free_fun) ctrl->free_fun(ctrl);
}

} // namespace controller
} // namespace openomf

#endif // OPENOMF_CONTROLLER_CONTROLLER_HPP

