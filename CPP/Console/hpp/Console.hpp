#ifndef OPENOMF_CONSOLE_CONSOLE_HPP
#define OPENOMF_CONSOLE_CONSOLE_HPP

// SDL_Event는 union (SDL_events.h). 전방 선언 시 union 사용.
union SDL_Event;

namespace openomf {
namespace game {

struct GameState;

} // namespace game

namespace console_detail {

struct ConsoleState {
    bool initialized;
    bool is_open;
    bool owns_input;

    ConsoleState()
        : initialized(false)
        , is_open(false)
        , owns_input(false)
    {
    }
};

static ConsoleState g_state;

} // namespace console_detail

namespace console {

using CommandFunc = int (*)(game::GameState* scene, int argc, char** argv);

inline bool console_init()
{
    if (console_detail::g_state.initialized) {
        return false;
    }
    console_detail::g_state = {};
    console_detail::g_state.initialized = true;
    return true;
}

inline void console_close()
{
    console_detail::g_state = {};
}

inline void console_event(game::GameState* /*scene*/, SDL_Event* /*event*/)
{
    // 1차 이식 단계에서는 SDL/Game 의 구체 타입에 의존하지 않고 noop 처리.
}

inline void console_render()
{
    // 렌더링 구현은 추후 Video/Game 모듈과 함께 이식.
}

inline void console_tick(game::GameState* /*gs*/)
{
    // stdin / 애니메이션 처리 등은 Game/Settings 이식 시 함께 구현.
}

inline void console_add_cmd(const char* /*name*/, CommandFunc /*func*/, const char* /*doc*/)
{
    // 명령 등록은 Game/Rec/Scene 이식 단계에서 실제 구현 예정.
}

inline void console_remove_cmd(const char* /*name*/)
{
}

inline void console_output_add(const char* /*text*/)
{
    // 현재 단계에서는 실제 출력 버퍼를持하지 않고 무시한다.
}

inline void console_output_addline(const char* /*text*/)
{
}

inline bool console_window_is_open()
{
    return console_detail::g_state.is_open;
}

inline void console_window_open()
{
    console_detail::g_state.is_open = true;
    console_detail::g_state.owns_input = true;
}

inline void console_window_close()
{
    console_detail::g_state.is_open = false;
    console_detail::g_state.owns_input = false;
}

} // namespace console
} // namespace openomf

#endif // OPENOMF_CONSOLE_CONSOLE_HPP

