#ifndef OPENOMF_EXTERNALLIBRARY_SDL2_HPP
#define OPENOMF_EXTERNALLIBRARY_SDL2_HPP

#include <SDL.h>

namespace openomf {
namespace external_library {

/** SDL2 라이브러리 정적 래퍼 (창/입력/오디오/타이머/파일시스템). */
class Sdl2 {
public:
    /** SDL 하위 시스템 초기화. */
    static int omf_sdl_init(Uint32 a_flags) { return SDL_Init(a_flags); }

    /** 지정한 SDL 하위 시스템만 초기화. */
    static int omf_sdl_init_sub_system(Uint32 a_flags) { return SDL_InitSubSystem(a_flags); }

    /** SDL 전체 종료. */
    static void omf_sdl_quit() { SDL_Quit(); }

    /** 지정한 SDL 하위 시스템만 종료. */
    static void omf_sdl_quit_sub_system(Uint32 a_flags) { SDL_QuitSubSystem(a_flags); }

    /** 마지막 오류 메시지 문자열 반환. */
    static const char* omf_sdl_get_error() { return SDL_GetError(); }

    /** 현재 플랫폼 이름 반환. */
    static const char* omf_sdl_get_platform() { return SDL_GetPlatform(); }

    /** 링크된 SDL 버전 정보 반환. */
    static void omf_sdl_get_version(SDL_version* a_ver) { SDL_GetVersion(a_ver); }

    /** 실행 파일 기준 경로 반환 (호출자가 SDL_free). */
    static char* omf_sdl_get_base_path() { return SDL_GetBasePath(); }

    /** 설정/저장용 사용자 경로 반환 (호출자가 SDL_free). */
    static char* omf_sdl_get_pref_path(const char* a_org, const char* a_app) {
        return SDL_GetPrefPath(a_org, a_app);
    }

    /** SDL이 할당한 메모리 해제. */
    static void omf_sdl_free(void* a_mem) { SDL_free(a_mem); }

    /** 창 생성. */
    static SDL_Window* omf_sdl_create_window(const char* a_title, int a_x, int a_y, int a_w, int a_h, Uint32 a_flags) {
        return SDL_CreateWindow(a_title, a_x, a_y, a_w, a_h, a_flags);
    }

    /** 창 파괴. */
    static void omf_sdl_destroy_window(SDL_Window* a_window) { SDL_DestroyWindow(a_window); }

    /** 창 클라이언트 영역 크기 설정. */
    static void omf_sdl_set_window_size(SDL_Window* a_window, int a_w, int a_h) {
        SDL_SetWindowSize(a_window, a_w, a_h);
    }

    /** 창 전체화면 모드 설정/해제. */
    static int omf_sdl_set_window_fullscreen(SDL_Window* a_window, Uint32 a_flags) {
        return SDL_SetWindowFullscreen(a_window, a_flags);
    }

    /** 화면 보호기 비활성화. */
    static void omf_sdl_disable_screen_saver() { SDL_DisableScreenSaver(); }

    /** OpenGL 컨텍스트 속성 설정. */
    static int omf_sdl_gl_set_attribute(SDL_GLattr a_attr, int a_value) {
        return SDL_GL_SetAttribute(a_attr, a_value);
    }

    /** 창에 대한 OpenGL 컨텍스트 생성. */
    static SDL_GLContext omf_sdl_gl_create_context(SDL_Window* a_window) {
        return SDL_GL_CreateContext(a_window);
    }

    /** OpenGL 컨텍스트 삭제. */
    static void omf_sdl_gl_delete_context(SDL_GLContext a_context) { SDL_GL_DeleteContext(a_context); }

    /** GL이 렌더링하는 영역의 픽셀 크기 반환. */
    static void omf_sdl_gl_get_drawable_size(SDL_Window* a_window, int* a_w, int* a_h) {
        SDL_GL_GetDrawableSize(a_window, a_w, a_h);
    }

    /** GL 더블 버퍼 스왑. */
    static void omf_sdl_gl_swap_window(SDL_Window* a_window) { SDL_GL_SwapWindow(a_window); }

    /** VSync 간격 설정 (0/1/-1). */
    static int omf_sdl_gl_set_swap_interval(int a_interval) { return SDL_GL_SetSwapInterval(a_interval); }

    /** 고해상도 타이머 현재 값. */
    static Uint64 omf_sdl_get_performance_counter() { return SDL_GetPerformanceCounter(); }

    /** 고해상도 타이머 주파수. */
    static Uint64 omf_sdl_get_performance_frequency() { return SDL_GetPerformanceFrequency(); }

    /** 지정 밀리초만큼 대기. */
    static void omf_sdl_delay(Uint32 a_ms) { SDL_Delay(a_ms); }

    /** 이벤트 큐에서 다음 이벤트 폴링. */
    static int omf_sdl_poll_event(SDL_Event* a_event) { return SDL_PollEvent(a_event); }

    /** 마우스 커서 표시 여부 설정. */
    static int omf_sdl_show_cursor(int a_toggle) { return SDL_ShowCursor(a_toggle); }

    /** 단순 메시지 박스 표시. */
    static int omf_sdl_show_simple_message_box(Uint32 a_flags, const char* a_title, const char* a_message,
                                               SDL_Window* a_window) {
        return SDL_ShowSimpleMessageBox(a_flags, a_title, a_message, a_window);
    }

    /** 뮤텍스 생성. */
    static SDL_mutex* omf_sdl_create_mutex() { return SDL_CreateMutex(); }

    /** 뮤텍스 파괴. */
    static void omf_sdl_destroy_mutex(SDL_mutex* a_mutex) { SDL_DestroyMutex(a_mutex); }

    /** 뮤텍스 잠금. */
    static int omf_sdl_lock_mutex(SDL_mutex* a_mutex) { return SDL_LockMutex(a_mutex); }

    /** 뮤텍스 잠금 해제. */
    static int omf_sdl_unlock_mutex(SDL_mutex* a_mutex) { return SDL_UnlockMutex(a_mutex); }

    /** 연결된 조이스틱 개수 반환. */
    static int omf_sdl_num_joysticks() { return SDL_NumJoysticks(); }

    /** 조이스틱 열기. */
    static SDL_Joystick* omf_sdl_joystick_open(int a_device_index) { return SDL_JoystickOpen(a_device_index); }

    /** 조이스틱 연결 여부. */
    static SDL_bool omf_sdl_joystick_get_attached(SDL_Joystick* a_joystick) {
        return SDL_JoystickGetAttached(a_joystick);
    }

    /** 조이스틱 닫기. */
    static void omf_sdl_joystick_close(SDL_Joystick* a_joystick) { SDL_JoystickClose(a_joystick); }

    /** 조이스틱 GUID 반환. */
    static SDL_JoystickGUID omf_sdl_joystick_get_guid(SDL_Joystick* a_joystick) {
        return SDL_JoystickGetGUID(a_joystick);
    }

    /** 조이스틱 GUID를 문자열로 변환. */
    static void omf_sdl_joystick_get_guid_string(SDL_JoystickGUID a_guid, char* a_psz_guid, int a_cb_guid) {
        SDL_JoystickGetGUIDString(a_guid, a_psz_guid, a_cb_guid);
    }

    /** 인덱스에 해당하는 조이스틱 이름 반환. */
    static const char* omf_sdl_joystick_name_for_index(int a_device_index) {
        return SDL_JoystickNameForIndex(a_device_index);
    }

    /** 조이스틱 축 개수. */
    static int omf_sdl_joystick_num_axes(SDL_Joystick* a_joystick) { return SDL_JoystickNumAxes(a_joystick); }

    /** 조이스틱 버튼 개수. */
    static int omf_sdl_joystick_num_buttons(SDL_Joystick* a_joystick) {
        return SDL_JoystickNumButtons(a_joystick);
    }

    /** 조이스틱 트랙볼 개수. */
    static int omf_sdl_joystick_num_balls(SDL_Joystick* a_joystick) { return SDL_JoystickNumBalls(a_joystick); }

    /** 조이스틱 햅 개수. */
    static int omf_sdl_joystick_num_hats(SDL_Joystick* a_joystick) { return SDL_JoystickNumHats(a_joystick); }

    /** 게임 컨트롤러 열기. */
    static SDL_GameController* omf_sdl_game_controller_open(int a_joystick_index) {
        return SDL_GameControllerOpen(a_joystick_index);
    }

    /** 게임 컨트롤러 닫기. */
    static void omf_sdl_game_controller_close(SDL_GameController* a_gamecontroller) {
        SDL_GameControllerClose(a_gamecontroller);
    }

    /** 게임 컨트롤러 연결 여부. */
    static SDL_bool omf_sdl_game_controller_get_attached(SDL_GameController* a_gamecontroller) {
        return SDL_GameControllerGetAttached(a_gamecontroller);
    }

    /** 게임 컨트롤러 축 값 반환. */
    static Sint16 omf_sdl_game_controller_get_axis(SDL_GameController* a_gamecontroller,
                                                   SDL_GameControllerAxis a_axis) {
        return SDL_GameControllerGetAxis(a_gamecontroller, a_axis);
    }

    /** 게임 컨트롤러 버튼 눌림 여부. */
    static Uint8 omf_sdl_game_controller_get_button(SDL_GameController* a_gamecontroller,
                                                   SDL_GameControllerButton a_button) {
        return SDL_GameControllerGetButton(a_gamecontroller, a_button);
    }

    /** 게임 컨트롤러에 대응하는 조이스틱 반환. */
    static SDL_Joystick* omf_sdl_game_controller_get_joystick(SDL_GameController* a_gamecontroller) {
        return SDL_GameControllerGetJoystick(a_gamecontroller);
    }

    /** 파일에서 게임 컨트롤러 매핑 추가. */
    static int omf_sdl_game_controller_add_mappings_from_file(const char* a_file) {
        return SDL_GameControllerAddMappingsFromFile(a_file);
    }

    /** 등록된 게임 컨트롤러 매핑 개수. */
    static int omf_sdl_game_controller_num_mappings() { return SDL_GameControllerNumMappings(); }

    /** 인덱스에 해당하는 게임 컨트롤러 이름 반환. */
    static const char* omf_sdl_game_controller_name_for_index(int a_joystick_index) {
        return SDL_GameControllerNameForIndex(a_joystick_index);
    }

    /** 조이스틱에서 햅틱 장치 열기. */
    static SDL_Haptic* omf_sdl_haptic_open_from_joystick(SDL_Joystick* a_joystick) {
        return SDL_HapticOpenFromJoystick(a_joystick);
    }

    /** 햅틱 럼블 지원 여부. */
    static int omf_sdl_haptic_rumble_supported(SDL_Haptic* a_haptic) {
        return SDL_HapticRumbleSupported(a_haptic);
    }

    /** 햅틱 럼블 초기화. */
    static int omf_sdl_haptic_rumble_init(SDL_Haptic* a_haptic) { return SDL_HapticRumbleInit(a_haptic); }

    /** 햅틱 럼블 재생. */
    static int omf_sdl_haptic_rumble_play(SDL_Haptic* a_haptic, float a_strength, Uint32 a_duration) {
        return SDL_HapticRumblePlay(a_haptic, a_strength, a_duration);
    }

    /** 햅틱 장치 닫기. */
    static void omf_sdl_haptic_close(SDL_Haptic* a_haptic) { SDL_HapticClose(a_haptic); }

    /** 스캔코드 이름 문자열 반환. */
    static const char* omf_sdl_get_scancode_name(SDL_Scancode a_scancode) {
        return SDL_GetScancodeName(a_scancode);
    }

    /** 이름에 해당하는 스캔코드 반환. */
    static SDL_Scancode omf_sdl_get_scancode_from_name(const char* a_name) {
        return SDL_GetScancodeFromName(a_name);
    }

    /** 현재 키보드 상태 배열 반환. */
    static const Uint8* omf_sdl_get_keyboard_state(int* a_numkeys) {
        return SDL_GetKeyboardState(a_numkeys);
    }

    /** 클립보드에 텍스트 존재 여부. */
    static SDL_bool omf_sdl_has_clipboard_text() { return SDL_HasClipboardText(); }

    /** 클립보드 텍스트 복사 (호출자가 SDL_free). */
    static char* omf_sdl_get_clipboard_text() { return SDL_GetClipboardText(); }

    /** IME/텍스트 입력 활성 여부. */
    static SDL_bool omf_sdl_is_text_input_active() { return SDL_IsTextInputActive(); }

    /** IME/텍스트 입력 시작. */
    static void omf_sdl_start_text_input() { SDL_StartTextInput(); }

    /** IME/텍스트 입력 중지. */
    static void omf_sdl_stop_text_input() { SDL_StopTextInput(); }

    /** 오디오 변환 구조체 초기화. */
    static int omf_sdl_build_audio_cvt(SDL_AudioCVT* a_cvt, SDL_AudioFormat a_src_format, Uint8 a_src_channels,
                                       int a_src_rate, SDL_AudioFormat a_dst_format, Uint8 a_dst_channels,
                                       int a_dst_rate) {
        return SDL_BuildAudioCVT(a_cvt, a_src_format, a_src_channels, a_src_rate, a_dst_format, a_dst_channels,
                                 a_dst_rate);
    }

    /** 오디오 변환 수행. */
    static int omf_sdl_convert_audio(SDL_AudioCVT* a_cvt) { return SDL_ConvertAudio(a_cvt); }

    /** SDL 할당자로 메모리 할당. */
    static void* omf_sdl_malloc(std::size_t a_size) { return SDL_malloc(a_size); }

    /** 메모리 복사. */
    static void omf_sdl_memcpy(void* a_dst, const void* a_src, std::size_t a_len) {
        SDL_memcpy(a_dst, a_src, a_len);
    }

    /** 파일에서 RWops 스트림 열기. */
    static SDL_RWops* omf_sdl_rw_from_file(const char* a_file, const char* a_mode) {
        return SDL_RWFromFile(a_file, a_mode);
    }

    /** RWops에 쓰기. */
    static std::size_t omf_sdl_rw_write(SDL_RWops* a_context, const void* a_ptr, std::size_t a_size,
                                        std::size_t a_num) {
        return SDL_RWwrite(a_context, a_ptr, a_size, a_num);
    }

    /** RWops 스트림 닫기. */
    static int omf_sdl_rw_close(SDL_RWops* a_context) { return SDL_RWclose(a_context); }
};

} // namespace external_library
} // namespace openomf

#endif // OPENOMF_EXTERNALLIBRARY_SDL2_HPP
