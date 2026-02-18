// Controller 모듈 빌드/연결 검증: Controller, NullController
#include "hpp/Controller.hpp"
#include "hpp/NullController.hpp"

using namespace openomf::controller;

static void test_controller_cmd_and_close()
{
    Controller ctrl;
    null_controller_init(&ctrl);

    CtrlEvent* ev = nullptr;
    controller_cmd(&ctrl, ACT_PUNCH | ACT_RIGHT, &ev);
    if (!ev) throw 1;
    if (ev->type != EVENT_TYPE_ACTION) throw 2;
    if (ev->action != (ACT_PUNCH | ACT_RIGHT)) throw 3;

    controller_close(&ctrl, &ev);
    if (!ev) throw 4;
    CtrlEvent* it = ev;
    while (it->next) it = it->next;
    if (it->type != EVENT_TYPE_CLOSE) throw 5;

    controller_free_chain(ev);
}

static void test_null_controller_tick_poll()
{
    Controller ctrl;
    null_controller_init(&ctrl);
    CtrlEvent* ev = nullptr;
    if (controller_tick(&ctrl, 0, &ev) != 0) throw 10;
    if (controller_dyntick(&ctrl, 0, &ev) != 0) throw 11;
    if (controller_poll(&ctrl, &ev) != 0) throw 12;
    controller_free(&ctrl);
}

int main()
{
    test_controller_cmd_and_close();
    test_null_controller_tick_poll();
    return 0;
}

