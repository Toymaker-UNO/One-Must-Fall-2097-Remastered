# 외부 라이브러리 목록 (C 트리 기준)

## 1. Purpose / Scope

- **목적**: `./C` 트리 내 모든 `*.h`, `*.c`를 재귀 스캔하여 사용 중인 외부 라이브러리와 해당 **함수명**을 정리한다.
- **범위**: `C/src` 소스에서 `#include <...>`로 참조하는 라이브러리, 및 `C/lib/bin` 아래 정적 라이브러리(`*.a`).
- **용도**: C→CPP 이식 시 의존성 파악, 최소 기능 셋·대체 가능성·라이선스/빌드 옵션 검토.

---

## 2. Classification Rules

| 구분 | 의미 |
|------|------|
| **Referenced** | `C/lib/include`에 헤더가 존재함 (빌드 시 include 경로에 포함 가능). |
| **Included** | `C/src` 소스에서 `#include <...>`로 실제 포함됨. |
| **Linked** | `C/lib/bin`에 `*.a` 또는 빌드 스크립트에서 링크되는 바이너리가 있음. |

- **C/lib/bin 아래 `*.a`**: 외부 라이브러리로 간주. (현재 스캔 시점에서 해당 경로에 `.a` 파일이 0개로 나왔으나, 빌드 환경에 따라 존재할 수 있음.)
- **`#include <...>`로만 참조되는 헤더**: 해당 헤더가 제공하는 라이브러리를 외부 라이브러리로 목록에 포함.

---

## 3. Summary Table

| 라이브러리 | Status | C에서 사용 | CPP에서 사용 | 비고 |
|------------|--------|------------|--------------|------|
| **SDL2** | Included, Linked | ✓ | - | 창/입력/오디오/타이머/파일시스템 |
| **SDL2_mixer** | Included, Linked | ✓ | - | 오디오 재생 (Mix_*) |
| **epoxy (OpenGL)** | Included, Linked | ✓ | - | gl* 함수 (OpenGL 3.3) |
| **libpng** | Included, Linked | ✓ | - | PNG 읽기/쓰기 |
| **zlib** | Referenced | (libpng 의존) | - | libpng가 사용 |
| **libconfuse** | Included, Linked | ✓ | - | 설정 파일 파싱 (cfg_*) |
| **argtable3** | Included (vendored) | ✓ | - | CLI 인자 파싱 (arg_*) |
| **enet** | Included, Linked | ✓ | - | 네트워크 (enet_*) |
| **opusfile** | Included, Linked | ✓ | - | Opus 오디오 (op_*) |
| **libxmp** | Included, Linked | ✓ | - | 모듈 음악 (xmp_*) |
| **C 표준 라이브러리** | Included | ✓ | - | stdio, stdlib, string, time, math 등 |
| **Windows API** | Included | ✓ | - | windows.h, shlwapi, shlobj, winsock, ws2tcpip 등 |
| **POSIX/기타** | Included | ✓ | - | dirent, libgen, getopt, sys/stat, unistd, fcntl 등 |
| **natpmp / miniupnpc** | Referenced (조건부) | ✓ (MINIUPNPC_FOUND) | - | NAT-PMP/UPnP (nat.c) |

---

## 4. External Libraries (Detailed)

### 4.1 SDL2

- **Detected headers**: `C/lib/include/SDL2/*.h` (SDL.h, SDL_video.h, SDL_events.h, SDL_audio.h, SDL_mutex.h, SDL_gamecontroller.h, SDL_joystick.h 등).
- **Included in C**: `<SDL.h>`, `<SDL_rect.h>`, `<SDL_mutex.h>`, `<SDL_gamecontroller.h>`, `<SDL_joystick.h>` 등.
- **Linked**: 빌드 시 SDL2 링크 (C/lib/bin에 .a 있을 수 있음).
- **Used symbols (Functions)**  
  - `SDL_Init`, `SDL_InitSubSystem`, `SDL_Quit`, `SDL_QuitSubSystem`  
  - `SDL_GetError`, `SDL_GetPlatform`, `SDL_GetVersion`, `SDL_GetBasePath`, `SDL_GetPrefPath`, `SDL_free`  
  - `SDL_CreateWindow`, `SDL_DestroyWindow`, `SDL_SetWindowSize`, `SDL_SetWindowFullscreen`, `SDL_DisableScreenSaver`  
  - `SDL_GL_SetAttribute`, `SDL_GL_CreateContext`, `SDL_GL_DeleteContext`, `SDL_GL_GetDrawableSize`, `SDL_GL_SwapWindow`, `SDL_GL_SetSwapInterval`  
  - `SDL_GetPerformanceCounter`, `SDL_GetPerformanceFrequency`, `SDL_Delay`  
  - `SDL_PollEvent`, `SDL_ShowCursor`, `SDL_ShowSimpleMessageBox`  
  - `SDL_CreateMutex`, `SDL_DestroyMutex`, `SDL_LockMutex`, `SDL_UnlockMutex`  
  - `SDL_NumJoysticks`, `SDL_JoystickOpen`, `SDL_JoystickClose`, `SDL_JoystickGetAttached`, `SDL_JoystickGetGUID`, `SDL_JoystickGetGUIDString`, `SDL_JoystickNameForIndex`, `SDL_JoystickNumAxes`, `SDL_JoystickNumButtons`, `SDL_JoystickNumBalls`, `SDL_JoystickNumHats`  
  - `SDL_GameControllerOpen`, `SDL_GameControllerClose`, `SDL_GameControllerGetAttached`, `SDL_GameControllerGetAxis`, `SDL_GameControllerGetButton`, `SDL_GameControllerGetJoystick`, `SDL_GameControllerAddMappingsFromFile`, `SDL_GameControllerNumMappings`, `SDL_GameControllerNameForIndex`  
  - `SDL_HapticOpenFromJoystick`, `SDL_HapticRumbleSupported`, `SDL_HapticRumbleInit`, `SDL_HapticRumblePlay`, `SDL_HapticClose`  
  - `SDL_GetScancodeName`, `SDL_GetScancodeFromName`, `SDL_GetKeyboardState`, `SDL_HasClipboardText`, `SDL_GetClipboardText`  
  - `SDL_IsTextInputActive`, `SDL_StartTextInput`, `SDL_StopTextInput`  
  - `SDL_BuildAudioCVT`, `SDL_ConvertAudio`, `SDL_malloc`, `SDL_memcpy`  
  - `SDL_RWFromFile`, `SDL_RWwrite`, `SDL_RWclose`
- **Types/Constants**: `SDL_Window`, `SDL_GLContext`, `SDL_Event`, `SDL_INIT_*`, `SDL_WINDOW_*`, `SDL_GL_*`, `SDL_MESSAGEBOX_ERROR`, `AUDIO_S16`, `AUDIO_U8`, `AUDIO_S16SYS` 등.
- **Where used (주요)**: `video/renderers/opengl3/sdl_window.c`, `gl3_renderer.c`, `engine.c`, `main.c`, `utils/log.c`, `utils/msgbox.c`, `resources/pathmanager.c`, `controller/joystick.c`, `controller/keyboard.c`, `controller/game_controller_db.c`, `controller/net_controller.c`, `game/game_state.c`, `game/gui/textinput.c`, `game/scenes/mainmenu/*.c`, `audio/backends/sdl/sdl_backend.c`, `audio/sources/opus_source.c`, `console/console.c`.
- **Migration note**: CPP 이식 시 창/입력/오디오/파일시스템 추상화(백엔드)로 분리 후 SDL2 백엔드 구현 권장.

---

### 4.2 SDL2_mixer

- **Detected headers**: `C/lib/include/SDL2/SDL_mixer.h`.
- **Included in C**: `<SDL_mixer.h>` (audio/backends/sdl/sdl_backend.c).
- **Used symbols (Functions)**  
  - `Mix_Init`, `Mix_Quit`, `Mix_OpenAudioDevice`, `Mix_CloseAudio`, `Mix_QuerySpec`, `Mix_AllocateChannels`  
  - `Mix_Volume`, `Mix_GroupAvailable`, `Mix_SetPanning`, `Mix_FadeInChannelTimed`, `Mix_FadeOutChannel`, `Mix_HaltMusic`  
  - `Mix_HookMusic`, `Mix_ChannelFinished`, `Mix_GetError`
- **Constants**: `SDL_MIXER_VERSION_ATLEAST`, `MIX_MAX_VOLUME`, `CHANNEL_MAX`, `AUDIO_S16SYS` 등.
- **Where used**: `audio/backends/sdl/sdl_backend.c`.
- **Migration note**: CPP 오디오 백엔드에서 SDL_mixer 대체(예: OpenAL, miniaudio) 검토 가능.

---

### 4.3 epoxy (OpenGL)

- **Detected headers**: `C/lib/include/epoxy/gl.h` 등.
- **Included in C**: `<epoxy/gl.h>` (video/renderers/opengl3/*).
- **Used symbols (Functions)**  
  - `glGetString`, `glGenTextures`, `glDeleteTextures`, `glTexParameteri`, `glTexImage2D`, `glTexSubImage2D`  
  - `glGenBuffers`, `glDeleteBuffers`, `glBufferData`, `glBufferSubData`, `glMapBufferRange`, `glFlushMappedBufferRange`, `glUnmapBuffer`  
  - `glGenVertexArrays`, `glDeleteVertexArrays`, `glBindVertexArray`, `glBindBuffer`, `glBindTexture`, `glBindFramebuffer`, `glBindBufferBase`  
  - `glActiveTexture`, `glVertexAttribPointer`, `glVertexAttribIPointer`, `glEnableVertexAttribArray`  
  - `glGenFramebuffers`, `glDeleteFramebuffers`, `glFramebufferTexture2D`, `glCheckFramebufferStatus`  
  - `glCreateShader`, `glShaderSource`, `glCompileShader`, `glGetShaderiv`, `glGetShaderInfoLog`, `glIsShader`, `glDeleteShader`  
  - `glCreateProgram`, `glAttachShader`, `glLinkProgram`, `glGetProgramiv`, `glGetAttachedShaders`, `glDeleteProgram`, `glGetProgramInfoLog`, `glIsProgram`  
  - `glGetUniformLocation`, `glGetUniformBlockIndex`, `glUniformMatrix4fv`, `glUniform1i`, `glUniform1ui`, `glUniformBlockBinding`, `glUseProgram`  
  - `glClearColor`, `glClear`, `glViewport`, `glDrawArrays`, `glMultiDrawArrays`, `glReadPixels`  
  - `glPixelStorei`, `glColorMask`, `glEnable`, `glDisable`, `glBlendEquation`
- **Where used**: `video/renderers/opengl3/helpers/*.c`, `gl3_renderer.c`, `sdl_window.c`.
- **Migration note**: CPP에서는 `IRenderer` 등 추상화 뒤 OpenGL 3.3 백엔드로 epoxy 또는 시스템 GL 로더 사용.

---

### 4.4 libpng

- **Detected headers**: `C/lib/include/png.h`, `C/lib/include/libpng16/png.h` 등.
- **Included in C**: `<png.h>` (utils/png_reader.c, utils/png_writer.c, formats/vga_image.c).
- **Used symbols (Functions)**  
  - `png_check_sig`, `png_create_read_struct`, `png_create_info_struct`, `png_destroy_read_struct`, `png_init_io`, `png_set_sig_bytes`, `png_read_info`, `png_get_image_width`, `png_get_image_height`, `png_get_color_type`, `png_get_bit_depth`, `png_get_rowbytes`, `png_malloc`, `png_read_image`, `png_free`  
  - `png_create_write_struct`, `png_destroy_write_struct`, `png_set_IHDR`, `png_write_info`, `png_write_row`, `png_write_end`  
  - `png_image_write_to_file`
- **Constants**: `PNG_LIBPNG_VER_STRING`, `PNG_COLOR_TYPE_*`, `PNG_INTERLACE_NONE`, `PNG_COMPRESSION_TYPE_BASE` 등.
- **Where used**: `utils/png_reader.c`, `utils/png_writer.c`, `formats/vga_image.c`.
- **Migration note**: CPP 이미지 로더에서 libpng 래퍼 또는 stb_image 등 대체 검토.

---

### 4.5 zlib

- **Detected headers**: `C/lib/include/zlib.h`, `zconf.h`.
- **Included in C**: 직접 `#include <zlib.h>` 없음. libpng가 내부적으로 사용.
- **Status**: Referenced (libpng 의존).
- **Migration note**: libpng 사용 시 함께 링크. CPP에서 PNG 대체 시 zlib 필요 여부 재검토.

---

### 4.6 libconfuse

- **Detected headers**: `C/lib/include/confuse.h`.
- **Included in C**: `<confuse.h>` (utils/config.c).
- **Used symbols (Functions)**  
  - `cfg_init`, `cfg_parse`, `cfg_free`, `cfg_print`  
  - `cfg_getint`, `cfg_getfloat`, `cfg_getbool`, `cfg_getstr`  
  - `cfg_setint`, `cfg_setfloat`, `cfg_setbool`, `cfg_setstr`
- **Types**: `cfg_t`, `cfg_opt_t`. **Constants**: `CFGF_IGNORE_UNKNOWN` 등.
- **Where used**: `utils/config.c`.
- **Migration note**: CPP 설정 모듈에서 YAML/JSON/자체 포맷으로 대체 가능.

---

### 4.7 argtable3

- **Detected headers**: 프로젝트 내 vendored `C/src/vendored/argtable3.h`.
- **Included in C**: `<argtable3.h>` (main.c).
- **Used symbols (Functions)**  
  - `arg_lit0`, `arg_str0`, `arg_int0`, `arg_file0`, `arg_end`  
  - `arg_nullcheck`, `arg_parse`, `arg_print_syntax`, `arg_print_glossary`, `arg_print_errors`, `arg_freetable`
- **Types**: `struct arg_lit`, `struct arg_str`, `struct arg_int`, `struct arg_file`, `struct arg_end`.
- **Where used**: `main.c`.
- **Migration note**: CPP에서는 CLI 파서(예: CLI11, docopt, 자체 파서)로 대체 가능. vendored이므로 이식 시 함께 이동.

---

### 4.8 enet

- **Detected headers**: `C/lib/include/enet/enet.h` 등.
- **Included in C**: `<enet/enet.h>` (main.c, game/scenes/mainmenu/menu_connect.c, menu_listen.c, game/scenes/lobby.c, controller/spec_controller.h, controller/net_controller.c).
- **Used symbols (Functions)**  
  - `enet_initialize`, `enet_deinitialize`  
  - `enet_host_create`, `enet_host_destroy`, `enet_host_connect`, `enet_host_service`, `enet_host_flush`  
  - `enet_socket_set_option`  
  - `enet_address_set_host`  
  - `enet_peer_send`, `enet_peer_reset`, `enet_peer_timeout`, `enet_peer_ping_interval`, `enet_peer_disconnect_later`  
  - `enet_packet_create`, `enet_packet_destroy`
- **Types/Constants**: `ENetHost`, `ENetPeer`, `ENetPacket`, `ENetEvent`, `ENET_PACKET_FLAG_RELIABLE`, `ENET_PACKET_FLAG_UNSEQUENCED`, `ENET_SOCKOPT_REUSEADDR` 등.
- **Where used**: `main.c`, `game/scenes/lobby.c`, `game/scenes/mainmenu/menu_connect.c`, `menu_listen.c`, `controller/net_controller.c`, `controller/spec_controller.c`.
- **Migration note**: CPP 네트워크 레이어에서 enet 래퍼 또는 다른 네트워크 라이브러리로 대체 검토.

---

### 4.9 opusfile (Opus)

- **Detected headers**: `C/lib/include/opus/opusfile.h` 등.
- **Included in C**: `<opusfile.h>` (audio/sources/opus_source.c).
- **Used symbols (Functions)**  
  - `op_open_file`, `op_free`, `op_read_stereo`, `op_raw_seek`
- **Where used**: `audio/sources/opus_source.c`.
- **Migration note**: CPP 오디오 소스에서 Opus 디코더 래퍼로 유지 또는 다른 코덱 백엔드와 통합.

---

### 4.10 libxmp

- **Detected headers**: `C/lib/include/xmp.h`.
- **Included in C**: `<xmp.h>` (audio/sources/psm_source.c).
- **Used symbols (Functions)**  
  - `xmp_create_context`, `xmp_free_context`, `xmp_load_module`, `xmp_release_module`  
  - `xmp_start_player`, `xmp_end_player`, `xmp_play_buffer`  
  - `xmp_get_module_info`, `xmp_set_player`
- **Constants**: `XMP_PLAYER_VOLUME`, `XMP_PLAYER_INTERP` 등.
- **Where used**: `audio/sources/psm_source.c`.
- **Migration note**: CPP 모듈 음악 재생 시 libxmp 래퍼 유지 또는 대체 포맷 지원 검토.

---

### 4.11 C 표준 라이브러리

- **Included in C**: `<assert.h>`, `<ctype.h>`, `<errno.h>`, `<limits.h>`, `<math.h>`, `<stdarg.h>`, `<stdbool.h>`, `<stddef.h>`, `<stdint.h>`, `<stdio.h>`, `<stdlib.h>`, `<string.h>`, `<time.h>`, `<inttypes.h>`, `<memory.h>`, `<setjmp.h>` (argtable3 등).
- **Used symbols**: 해당 헤더의 표준 함수 전반 (printf, malloc, memcpy, strlen, sin/cos 등). 생략.
- **Migration note**: CPP에서는 `std::` 및 표준 라이브러리로 대체.

---

### 4.12 Windows API

- **Included in C**: `<windows.h>`, `<shlwapi.h>`, `<shlobj.h>`, `<winsock.h>`, `<ws2tcpip.h>` (경로/파일/네트워크).
- **Used symbols**: `PathFileExistsA`, `SHCreateDirectoryEx` 등 (io.c, pathmanager.c). 기타 Win32 API는 빌드/조건부 사용.
- **Where used**: `utils/io.c`, `utils/scandir.c`, `resources/pathmanager.c`, `game/utils/nat.h` (ws2tcpip), vendored argtable3 (windows.h).
- **Migration note**: CPP에서 플랫폼별 추상화(파일/경로/소켓)로 분리 후 Windows 구현체에서만 사용.

---

### 4.13 POSIX / 기타 플랫폼

- **Included in C**: `<dirent.h>`, `<libgen.h>`, `<sys/stat.h>`, `<unistd.h>`, `<fcntl.h>`, `<getopt.h>`, `<arpa/inet.h>` (호스트/빌드에 따라 사용).
- **Used symbols**: 디렉터리 열기/읽기, mkdir, open/close 등. 조건부 컴파일로 플랫폼별 분기.
- **Where used**: `utils/scandir.c`, `console/console.c`, `game/utils/serial.c`, `resources/pathmanager.c` 등.
- **Migration note**: CPP에서는 `std::filesystem` 및 플랫폼 추상화로 대체.

---

### 4.14 natpmp / miniupnpc

- **Detected headers**: `nat.h`에서 `<natpmp.h>`, `<miniupnpc/miniupnpc.h>`, `<miniupnpc/portlistingparse.h>`, `<miniupnpc/upnpcommands.h>` (조건부 `MINIUPNPC_FOUND`).
- **Included in C**: `game/utils/nat.h`, `game/utils/nat.c`.
- **Used symbols**: `UPNP_GetListOfPortMappings` 등 (nat.c). 기타 NAT-PMP/UPnP API.
- **Status**: Referenced/Included (옵션 빌드).
- **Migration note**: CPP 네트워크/NAT 기능에서 동일 조건부 도입 또는 제거 결정.

---

## 5. C/lib/bin (*.a)

- **의미**: 해당 경로의 `*.a`는 외부 라이브러리의 **정적 라이브러리**로 간주한다.
- **현재 스캔**: 프로젝트 루트 기준 `C/lib/bin` 아래 `*.a` 파일이 0개로 검색됨. (빌드 환경·CMake 등에서 다른 경로에 두거나 시스템 라이브러리를 링크할 수 있음.)
- **C/lib/include와의 대응**:  
  - SDL2, SDL2_mixer, epoxy, libpng, zlib, libconfuse, enet, opus/opusfile, libxmp, vorbis, ogg 등 헤더가 `C/lib/include`에 있으면, 대응하는 정적/동적 라이브러리가 빌드 시 링크될 수 있음.  
  - 실제 링크되는 바이너리 목록은 빌드 스크립트(CMakeLists.txt, Makefile 등)를 확인하면 정확히 알 수 있다.

---

## 6. Appendix

- **스캔 대상**: `./C` 재귀 하위의 모든 `*.h`, `*.c`.
- **스캔 방법**: `#include <...>` 패턴 및 라이브러리별 함수 호출 패턴 검색 (grep).
- **생성 일자**: 2025년 기준 스캔 결과로 작성. 빌드 설정·플랫폼에 따라 실제 링크되는 라이브러리는 다를 수 있음.
- **갱신**: C 트리 또는 `C/lib` 변경 시 이 문서와 사용 함수 목록을 재스캔하여 갱신하는 것을 권장한다.
