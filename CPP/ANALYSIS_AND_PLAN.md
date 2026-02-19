# C 소스 분석 및 CPP 이식 계획

## 1. C 디렉터리 소스 분석 요약

### 1.1 전체 규모

| 구분 | 수량 |
|------|------|
| **전체 라인 수 (*.c + *.h)** | **약 55,200 라인** |
| C 소스 파일 (*.c) | 약 189개 |
| C 헤더 파일 (*.h) | 약 207개 |
| 진입점 | `main.c` |
| 빌드 | `C/00_build.ps1` (PowerShell, gcc/g++, vcpkg lib) |

### 1.2 모듈별 분류 (C/src 기준)

| 모듈 | 경로 | 역할 | 대략 파일 수 (.c) | 대략 라인 수 | 외부 의존 |
|------|------|------|-------------------|--------------|------------|
| **main** | src/main.c | CLI(argtable3), pm_init, engine_init/run | 1 | 약 290 | utils, engine, resources |
| **engine** | src/engine.c,h | 초기화·메인 루프 | 1 | 약 380 | video, audio, game_state, console, controller, formats, resources, utils |
| **video** | src/video/ | 비디오 API, surface, vga_*, renderers (null, opengl3) | ~25 | 약 2,750 | utils, **SDL2, epoxy** |
| **audio** | src/audio/ | 오디오 API, backends(sdl,null), sources(psm,opus) | ~6 | 약 930 | utils, **SDL2_mixer, libxmp, opusfile** |
| **controller** | src/controller/ | 입력/네트워크 (keyboard, joystick, ai, net, rec, spec) | ~8 | 약 4,950 | **SDL2, enet**, game 타입 |
| **console** | src/console/ | 개발자 콘솔, 명령 등록/실행 | ~2 | 약 840 | utils, game_state |
| **game** | src/game/ | game_state, protos, objects, gui, scenes | ~70+ | 약 24,800 | video, audio, controller, resources, formats, utils |
| **formats** | src/formats/ | 파일 포맷 (palette, pcx, bk, af, animation, rec, …) | ~35+ | 약 8,800 | utils, **libpng** (일부) |
| **resources** | src/resources/ | pathmanager, ids, 로더·매니저 (bk, af, fonts, sounds, …) | ~18 | 약 2,000 | formats, utils |
| **utils** | src/utils/ | allocator, array, vector, str, log, config, io, png_*, … | ~30 | 약 3,900 | **libpng**, **libconfuse**, C 표준, Windows/POSIX |
| **vendored** | src/vendored/ | argtable3 (CLI) | 1 | 약 5,500 | 없음 (자체) |

- **CPP/ExternalLibrary** 에는 위 표의 굵은 외부 라이브러리(SDL2, epoxy, SDL2_mixer, libxmp, opusfile, enet, libpng, libconfuse 등)에 대한 hpp 래퍼가 이미 있음.
- C 이식 시 **외부 호출은 반드시 CPP/ExternalLibrary/hpp** 를 통하도록 할 것.

### 1.3 라인 수 분석

- **집계 대상**: `C/src` 재귀 하위의 모든 `*.c`, `*.h` 파일.
- **집계 방법**: 파일별 텍스트 줄 수를 합산 (빈 줄·주석 포함).
- **모듈 구분**: `src` 기준 상위 디렉터리 한 단계로 묶어서 합산 (예: `src/game/` 하위 전부 → game).

**집계 결과 (모듈별 라인 수):**

| 모듈 | 라인 수 | 비고 |
|------|--------|------|
| game | 24,832 | 씬·GUI·오브젝트·프로토 포함 |
| formats | 8,779 | 파일 포맷·reader/writer |
| vendored | 5,534 | argtable3 |
| controller | 4,953 | 입력·네트워크·AI·녹화 등 |
| utils | 3,897 | allocator, array, str, config, io 등 |
| video | 2,746 | surface, renderers(opengl3, null) |
| resources | 2,034 | pathmanager, 로더·매니저 |
| audio | 925 | 백엔드·소스(PSM, Opus) |
| console | 837 | 콘솔·명령 |
| engine | 384 | engine.c + engine.h |
| main | 285 | main.c |
| **합계** | **55,206** | |

- 위 1.1·1.2 표의 "약 N 라인" 값은 이 집계를 반올림한 값이다.

---

## 2. 의존성 순서 (이식 시 권장 빌드/테스트 순서)

```
[0] ExternalLibrary     (이미 존재, 빌드/연결 검증 완료)
[1] Utils              (기반: allocator, array, str, log, config, io, …)
[2] Formats             (Utils 의존, 내부 reader/writer 등)
[3] Resources           (Formats, Utils 의존)
[4] Video               (Utils, ExternalLibrary[SDL2, Epoxy] 의존)
[5] Audio               (Utils, ExternalLibrary[SDL2_mixer, LibXmp, Opusfile] 의존)
[6] Controller          (Utils, ExternalLibrary[SDL2, Enet], Game 타입 일부)
[7] Console             (Utils, Game 타입)
[8] Game                (Video, Audio, Controller, Resources, Formats, Utils)
[9] Engine + main      (전체 통합)
```

- **CPP 쪽 옵션 정책**: 실행 파일에서 명령줄 인자(argv)로 옵션을 받지 않는다. 여러 다양한 옵션은 **JSON 설정 파일**로 넣기로 한다. 설정 파일 이름은 **`omf_config.json`** 으로 통일하며, JSON 형식(키/구조)은 추후 설계·구현 시 정하기로 한다. 옵션 관련 설계/구현 시 이 정책을 반영할 것. (C의 vendored/argtable3 는 CPP 에서 사용하지 않음.)

- **모듈별 C++ 빌드/컴파일 시험**: 각 모듈을 ExternalLibrary 와 동일한 방식으로 `모듈명.build.ps1` + `모듈명_test.main.cpp` 로 독립 컴파일 가능하게 구성.

---

## 2.5 중간 점검 (현재 상태)

- **이 과정이 끝나면 CPP에서 OMF 게임을 빌드·플레이할 수 있는가?**  
  **아니요.** 현재 CPP에는 **Console**, **Game**(game_state, protos, objects, gui, scenes), **Engine**, **main**이 아직 이식되지 않았으므로, CPP만으로 **실행 가능한 게임 한 벌**을 빌드하는 타깃은 없다.  
  **지금 가능한 것**: `.\CPP\00_build_all_modules.ps1` 실행 시 ExternalLibrary → Utils → Formats → Resources → Video → Audio → Controller까지 **모듈 단위**로 C++ 컴파일·링크·테스트 실행이 되며, 각 모듈의 동작만 검증된다. 전체 게임 런처/엔진/메인 루프는 없다.

- **컴파일/빌드는 C++ 컴파일러·빌더를 사용하는가?**  
  **예.** 각 모듈의 `*.test.build.ps1`은 **g++(C++ 컴파일러)**를 사용하며, `-std=c++17` 등으로 C++ 표준을 지정하고 링크도 g++로 수행한다. 따라서 CPP 이식이 완료된 뒤 전체 게임을 빌드할 때도 **C++ 컴파일러/빌더**를 사용하게 된다.

- **정리**:  
  - **게임 빌드·플레이**: Console → Game → Engine → main 이식 및 통합 빌드/실행 타깃이 추가된 뒤에야 CPP에서 OMF 게임을 빌드·플레이할 수 있다.  
  - **빌드 도구**: 이미 CPP 쪽은 C++(g++) 기반이며, 앞으로도 C++ 컴파일러/빌더를 사용한다.

---

## 3. 권장 작업 순서

1. **모듈별 빌드/컴파일 시험 인프라 확립**
   - CPP 아래에 `Utils`, `Formats`, `Resources`, `Video`, `Audio`, `Controller`, `Console`, `Game` 등 모듈 디렉터리 생성.
   - 각 모듈에 `모듈명.build.ps1`, `모듈명_test.main.cpp` 두어 ExternalLibrary 와 같이 **모듈 단위로 컴파일 시험** 가능하게 함.
   - 의존 모듈은 `-I` 로 CPP 경로만 추가; 실제 구현은 단계적으로 C → C++ 이식.

2. **Utils 모듈부터 C++ 이식**
   - C의 `utils` (allocator, array, vector, str, log, config, io, compat, cp437 등)를 C++로 옮기고, **외부 라이브러리 사용처는 ExternalLibrary/hpp** 로 교체 (예: config → LibConfuse.hpp, png → LibPng.hpp).
   - Utils 빌드 스크립트로 해당 모듈만 컴파일·링크하여 시험.

3. **Formats → Resources 순차 이식**
   - Formats는 아래 **3.1 Formats 내부 이식 순서**대로 이식하고 Utils(및 필요 시 ExternalLibrary)만 의존하도록 유지.
   - Resources는 pathmanager, ids, bk_loader, af_loader 등 Formats·Utils 기반으로 이식.

### 3.1 Formats 내부 이식 순서 (상세)

의존 관계에 따라 아래 순서로 진행한다. 각 항목은 CPP/Formats/hpp 에 .hpp 로 구현.

| 순서 | 항목 | C 대응 | 비고 |
|------|------|--------|------|
| 1 | Reader, Writer, MemReader, MemWriter | internal/reader, writer, memreader, memwriter | 파일/메모리 I/O 기반 |
| 2 | Error | formats/error.h | SdError enum |
| 3 | Palette | palette.h/c | VgaColor, VgaPalette, VgaRemapTables, load/save, remaps_init |
| 4 | VgaImage | vga_image.h/c | create, copy, free (PNG·RGBA decode는 추후) |
| 5 | Pcx | pcx.h/c | PcxFile, pcx_load, pcx_load_font, pcx_font_decode, pcx_free |
| 6 | Bk (1차) | bk.h/c | BkFile, create, free, load_from_pcx, get_background, get_palette |
| 7 | ColCoord | colcoord.h | sd_coord 구조체 |
| 8 | Sprite | sprite.h/c | sd_sprite, create, copy, free, load, save (vga_decode 등 추후) |
| 9 | Animation | animation.h/c | sd_animation, create, copy, free, load, save, coord/extra_string 접근 |
| 10 | Bkanim | bkanim.h/c | sd_bk_anim, create, copy, free, load, save |
| 11 | Bk (2차) | bk.h/c | sd_bk_load, sd_bk_save (전체 .BK 파일) |
| 12 | Move | move.h/c | sd_move, create, copy, free, load, save |
| 13 | AF | af.h/c | sd_af_file, create, copy, free, load, save |

- **현재까지 완료**: 1～13 (Reader, Writer, MemReader, MemWriter, Error, Palette, VgaImage, Pcx, Bk 1차·2차, ColCoord, Sprite, Animation, Bkanim, Move, AF).
- **Formats 빌드·테스트 검증 완료**: `formats.test.build.ps1`로 컴파일·링크 후 `formats.test.exe` 자동 실행으로 통과 확인. `00_build_all_modules.ps1`에서 ExternalLibrary → Utils → Formats 순서로 빌드 통과.
- **Resources 모듈 이식 완료**: Ids, PathManager(최소), BkLoader, AfLoader. `resources.test.build.ps1` 및 `00_build_all_modules.ps1`에 반영.
- **Video 모듈 이식 (1차 완료)**: Enums, Color, Image, Surface(Formats VgaImage 연동), Renderer 인터페이스, NullRenderer, Video(video_scan_renderers, video_init, video_close). `video.test.build.ps1` 및 `00_build_all_modules.ps1`에 반영.
- **Audio 모듈 이식 (1차 완료)**: MusicSource, AudioBackend, NullBackend, Audio(audio_scan_backends, audio_get_backend_count/info, audio_init, audio_close). `audio.test.build.ps1` 및 `00_build_all_modules.ps1`에 반영.
- **Controller 모듈 이식 (1차 완료)**: ActionFlags, EventType, Controller(축약 버전), NullController(null_controller_init, tick/poll/rumble/rewind/free no-op). `controller.test.build.ps1` 및 `00_build_all_modules.ps1`에 반영.
- **Console 모듈 이식 (1차 stub 완료)**: `console.test.build.ps1`, `console.test.main.cpp`, 초기화/창 열기·닫기/출력 API stub. `00_build_all_modules.ps1`에 반영.
- **Game/protos 타입 이식 완료**: Utils에 `Vec.hpp`(vec2i, vec2f) 추가. Game에 `CommonDefines.hpp`, `FightStats.hpp`, `Serial.hpp`, `TickTimer.hpp`, `PlayerState.hpp`, `Object.hpp`, `Scene.hpp`, `Intersect.hpp`(선언만) 추가.
- **Game/objects·scenes API 이식 완료**: `ArenaConstraints.hpp`, `Har.hpp`(상수·enum), `TickTimerApi.hpp`, `SerialApi.hpp`, `PlayerApi.hpp`(player_create/player_free stub), `ObjectApi.hpp`(object_create, object_free, object_set_pos/get_pos, object_dynamic_tick 등), `SceneApi.hpp`(scene_create_empty, scene_init, scene_free, scene_event/render/tick 등), `Arena.hpp`(arena_create, arena_get_state 등 stub). 리소스 로드(scene_create에서 BK/AF 로드), object_render, har_create 등은 미이식.
- **Engine + main 이식 완료**: `CPP/Engine/hpp/Engine.hpp`에 풀 이벤트 루프·씬 틱·렌더 연동( SDL_PollEvent, 콘솔 열기/닫기, game_state_handle_event, game_state_static_tick/dynamic_tick, video_render_prepare/finish, game_state_render, console_render). `CPP/Game/hpp/GameStateApi.hpp`에 handle_event, render, static_tick, dynamic_tick, get_framebuffer_options, ms_per_dyntick, palette_transform 스텁. `CPP/Video/hpp/Video.hpp`에 video_render_prepare/video_render_finish. `CPP/Engine/main.cpp`에서 SDL_Init → PathManager::set_resource_base(SDL_GetBasePath()) → engine_init/run/close → SDL_Quit. `engine.build.ps1`로 `omf.exe` 빌드( SDL2 정적 링크 시 MinGW에서 추가 시스템 라이브러리 필요할 수 있음).

### 3.2 현재까지 안 한 것 / 추후 이식할 것 정리

#### 3.2.1 현재까지 안 한 것 (미이식 모듈·기능)

아직 CPP로 이식하지 않은 **모듈 전체** 또는 **모듈 단위 기능**이다.

| 구분 | 항목 | C 대응 | 비고 |
|------|------|--------|------|
| **모듈** | Audio (1차 완료, SDL/PSM/Opus 미포함) | src/audio/ | 백엔드(sdl,null), PSM/Opus 소스 등 추후 이식 |
| **모듈** | Controller (1차 완료, 키보드/네트워크 등 미포함) | src/controller/ | keyboard, joystick, ai, net, rec, spec, SDL2, Enet 등 추후 이식 |
| **모듈** | Console (1차 stub 완료) | src/console/ | CPP/Console 에 기본 상태 및 API stub 존재. 실제 콘솔 명령, 렌더링, 히스토리/버퍼 등은 Game/Video 이식 단계에서 구현 필요. |
| **모듈** | Game (protos·objects·scenes API 1차 완료) | src/game/ | CPP/Game 에 타입·ObjectApi·SceneApi·Arena·Har 상수·TickTimer/Serial/Player stub 이식됨. scene_create(리소스 로드), object_render, har_create, gui, 개별 씬(mainmenu, mechlab 등) 로직은 미이식. |
| **모듈** | Engine + main (이벤트·씬·렌더·리소스 연동 완료) | src/engine.c, main.c | CPP/Engine: engine_run에 SDL_PollEvent, 콘솔 Tab/Backquote, game_state_handle_event, static/dynamic_tick, video_render_prepare/finish, game_state_render, console_render. main에서 SDL_Init → PathManager::set_resource_base → engine_init/run/close → SDL_Quit. omf_config.json·settings는 추후. |
| **Formats** | PNG·RGBA 디코드 | vga_image, rgba 등 | VgaImage 확장, RGBA 이미지 (필요 시) |
| **Formats** | Sprite vga_decode 등 | sprite.c | 스프라이트 디코딩 확장 |
| **Resources** | PathManager 전체 | pathmanager.c | SDL 기반 pm_init, pm_get_local_base_dir, pm_validate_resources 등 |
| **Resources** | 기타 로더·매니저 | fonts, languages, pilots, scores, sgmanager, sounds_loader, trnmanager 등 | 리소스 확장 |

#### 3.2.2 추후 이식할 것 (이미 시작한 모듈 내 보류 항목)

이미 1차 이식이 끝난 모듈 안에서, **의존성·범위** 때문에 나중 단계로 미룬 항목이다.

| 모듈 | 항목 | C 대응 | 미룬 이유 |
|------|------|--------|-----------|
| **Video** | vga_state | vga_state.h/c | 팔레트/리맵 현재 상태 관리 → 실제 렌더러 이식 후 연동 |
| **Video** | vga_palette (비디오 쪽) | vga_palette.h/c | tint/mix/light_range 등 → 렌더링 파이프라인과 함께 이식 |
| **Video** | vga_remap | vga_remap.h/c | 리맵 테이블 → vga_state·렌더러와 함께 이식 |
| **Video** | OpenGL3 렌더러 | renderers/opengl3/ | gl3_renderer, sdl_window, texture, shaders, FBO 등, Epoxy 의존 → Null로 API 검증 후 별도 단계 |
| **Video** | video_draw | video.c | video_draw(Surface, x, y) 추가·NullRenderer에 no-op. 씬 배경 그리기 연동됨. video_draw_remap/offset/size 등은 추후 |
| **Video** | damage_tracker | damage_tracker.h/c | 더티 영역 등 → 렌더링·게임 연동 시 이식 |
| **Video** | image_line, image_rect 등 | image.c | Image 확장 → 필요 시 이식 |
| **Video** | surface_sub, surface_write_png 등 | surface.c | Surface 고급 연산 → 필요 시 이식 |
| **Audio** | SDL 백엔드 | backends/sdl/sdl_backend | SDL2_mixer 의존 → 추후 이식 |
| **Audio** | PSM/Opus 소스, audio_play_sound, audio_play_music 등 | sources/psm_source, opus_source, audio.c | LibXmp, Opusfile, Resources(sounds_loader, pathmanager) 의존 → 추후 이식 |
| **Controller** | Keyboard/Joystick/AI/Net/Rec/Spec 컨트롤러 | keyboard, joystick, ai_controller, net_controller, rec_controller, spec_controller | SDL2, Enet, Game 타입(game_state, har, object) 의존 → Game/Net 이식 단계에서 함께 이식 |
| **Game** | scene_create(리소스 로드), object_render, har_create, 개별 씬(mainmenu, arena 등) | scene.c, object.c, har.c, scenes/*.c | scene_create_with_resources(BK/PCX 로드·배경 Surface)·scene_render(배경 video_draw)·scene_free 연동 완료. object_render, har_create, 개별 씬 로직은 추후 |
| **Engine** | 풀 메인 루프·씬 틱·렌더·리소스 연동 완료. omf_config.json·settings 추후 | engine.c, main.c | engine_run: 이벤트 루프(SDL_QUIT, 콘솔 열기/닫기, game_state_handle_event), static/dynamic_tick, video_render_prepare/finish, game_state_render, console_render. main: SDL_Init → PathManager::set_resource_base → engine_init/run/close → SDL_Quit. C\lib\bin에서 SDL2 정적 링크 시 MinGW 추가 라이브러리 필요할 수 있음. |

- **정리**: “현재까지 안 한 것”은 **아직 손대지 않은 모듈/기능**, “추후 이식할 것”은 **이미 뼈대를 이식한 모듈 안에서 다음 단계로 미룬 항목**이다.

4. **Video / Audio / Controller**
   - Video: surface, image, vga_state, renderer 인터페이스, opengl3/null 렌더러 → ExternalLibrary(Sdl2, Epoxy) 사용.
   - Audio: 백엔드 인터페이스 + sdl/null 백엔드, PSM/Opus 소스 → ExternalLibrary(Sdl2Mixer, LibXmp, Opusfile).
   - Controller: controller_t, keyboard, joystick, net → ExternalLibrary(Sdl2, Enet).

5. **Console → Game → Engine → main**
   - 콘솔 명령, game_state, scene, object, gui, scenes 를 단계적으로 이식하고, 마지막에 engine + main 통합.

---

## 4. 모듈별 빌드/컴파일 시험 구조

- **ExternalLibrary** (기존)
  - `CPP/ExternalLibrary/external_library.test.build.ps1`
  - `CPP/ExternalLibrary/external_library.test.main.cpp`
- **그 외 모듈** (동일 패턴)
  - `CPP/<ModuleName>/` 아래
    - `<module_name>.build.ps1`  … 해당 모듈 + 의존 모듈 include/링크만으로 컴파일
    - `<module_name>_test.main.cpp` … 최소 한 개의 해당 모듈 헤더 include 및 `main()` (초기에는 stub으로 컴파일만 검증)
  - 의존성: 스크립트 내에서 `-I ../ExternalLibrary`, `-I ../Utils` 등으로 상대 경로 지정.

- **전체 모듈 빌드 시험**: `CPP/00_build_all_modules.ps1` 에서 각 모듈의 `*.build.ps1` 을 의존 순서대로 호출하여 모두 컴파일되는지 확인.

### 4.1 이미 구성된 모듈

| 모듈 | 디렉터리 | 빌드 스크립트 | 테스트 main |
|------|----------|----------------|-------------|
| ExternalLibrary | CPP/ExternalLibrary/ | external_library.test.build.ps1 | external_library.test.main.cpp |
| Utils | CPP/Utils/ | utils.test.build.ps1 | utils.test.main.cpp |
| Formats | CPP/Formats/ | formats.test.build.ps1 | formats.test.main.cpp |
| Resources | CPP/Resources/ | resources.test.build.ps1 | resources.test.main.cpp |
| Video | CPP/Video/ | video.test.build.ps1 | video.test.main.cpp |
| Audio | CPP/Audio/ | audio.test.build.ps1 | audio.test.main.cpp |
| Controller | CPP/Controller/ | controller.test.build.ps1 | controller.test.main.cpp |
| Console | CPP/Console/ | console.test.build.ps1 | console.test.main.cpp |
| Game | CPP/Game/ | game.test.build.ps1 | game.test.main.cpp |
| Engine | CPP/Engine/ | engine.build.ps1 | main.cpp → omf.exe |

- **전체 실행**: 프로젝트 루트 또는 `CPP` 에서 `.\CPP\00_build_all_modules.ps1` 실행.

### 4.2 새 모듈 추가 방법

1. `CPP/<ModuleName>/` 폴더 생성.
2. `<module_name>.build.ps1` 작성 (필요 시 `-I ../ExternalLibrary`, `-I ../Utils` 등 의존 모듈 include 경로 추가).
3. `<module_name>_test.main.cpp` 작성 (최소한 해당 모듈 hpp 하나 include 및 `int main()`).
4. `CPP/00_build_all_modules.ps1` 의 `$Modules` 배열에 `@{ Name = "..."; Script = "..."; Dir = "..." }` 추가 (의존 순서 유지).

---

## 5. 참고

- C 쪽 상세 구조: `C/ARCHITECTURE.md`
- 외부 라이브러리 목록·사용처: `CPP/ExternalLibrary/List.md`
- C 빌드 스크립트: `C/00_build.ps1`
- **규칙**: C 디렉터리는 수정하지 않고, 모든 변경은 CPP 이하에서만 수행. 외부 라이브러리는 `CPP/ExternalLibrary/hpp` 를 통해 사용.
- **옵션 설정**: CPP 에서는 명령줄 인자 대신 **`omf_config.json`** 사용. JSON 형식은 추후 옵션 설계 시 정의.

---

## 6. 현재 진행사항·오류·해결 과제

### 6.1 현재까지 진행사항 (최근 반영)

| 구분 | 내용 |
|------|------|
| **이벤트 루프** | `engine_run`: SDL_PollEvent, SDL_QUIT, 콘솔 Tab/Backquote 열기·닫기, WINDOWEVENT(minimize/restore), `game_state_handle_event` / `console_event` 분기, `enable_screen_updates` |
| **씬 틱** | `game_state_static_tick` / `game_state_dynamic_tick` 호출, `new_state` 치환 시 `game_state_clone_free`·해제, `game_state_palette_transform`(no-op), `STATIC_TICKS_MS`·`TICK_EXPIRY_MS`·`MAX_TICKS_PER_FRAME` 적용 |
| **렌더** | `video_render_prepare` / `video_render_finish`, `game_state_render` → `scene_render` + `scene_render_overlay`, `video_draw(Surface, x, y)` 추가 |
| **리소스 로드** | main: SDL_Init → `PathManager::set_resource_base(SDL_GetBasePath())` → engine. `scene_create_with_resources`: PathManager·BkLoader로 BK(또는 PCX) 로드, 배경용 `Surface` 캐시, `scene_free`에서 배경·bk_data 해제 |
| **씬** | `Scene`에 `background_surface`, `scene_render`에서 배경 `video_draw` 후 render 콜백. 씬 ID별 리소스 매핑(SCENE_MENU→BK_MENU, SCENE_LOBBY→PCX_NETARENA 등). Engine에서 `scene_create_with_resources`·`scene_init`·`scene_free` 연동 |
| **사운드 API** | `audio_play_sound(id, volume, panning, pitch)`, `audio_play_music(resource_id)` 추가. Null 백엔드 no-op. 실제 재생은 sounds_loader/PSM·Opus 이식 후 |
| **입력** | `KeyboardController.hpp`: SDL 키보드 → ACT_* 플래그. `GameState::menu_ctrl` 연결, `game_state_tick_controllers`·`game_state_static_tick`에서 `controller_tick(menu_ctrl)` 호출. Engine에서 keyboard_controller 생성·해제 |
| **Formats** | `SdError`를 `openomf::formats` 네임스페이스로 이동해 BkLoader/SceneApi와 일치시킴 |

### 6.2 발생한 오류 및 조치

| 오류 | 원인 | 조치 |
|------|------|------|
| `'SdError' in namespace 'openomf::formats' does not name a type` (BkLoader/SceneApi) | `Error.hpp`의 `SdError`가 전역 네임스페이스에 있음 | `Error.hpp`에서 `SdError`를 `namespace openomf { namespace formats { ... } }` 안으로 이동 |
| Engine 링크 실패: `undefined reference to __imp_SetupDiGetClassDevsA`, `ImmGetContext`, `CoInitializeEx` 등 | `C\lib\bin`의 **SDL2 정적 라이브러리**와 MinGW 링크 시 Windows 시스템 라이브러리 미지정 | `engine.build.ps1`에 `-lsetupapi`, `-limm32`, `-lole32`, `-lopengl32` 등 추가. 환경에 따라 추가 lib 필요하거나 **동적 SDL2**(.dll) 사용 필요 |

### 6.3 해결해야 할 문제점

| 우선순위 | 문제 | 설명 | 권장 조치 |
|----------|------|------|-----------|
| **높음** | Engine(omf.exe) 링크 실패 | SDL2 정적 링크 시 MinGW에서 Windows 심볼 미해결. 컴파일은 성공, 링크만 실패 | (1) SDL2 동적 링크로 전환(`libSDL2.dll.a` + dll 배치) 또는 (2) 사용 중인 MinGW/SDL2 빌드에 맞는 시스템 라이브러리 목록 확정 후 `engine.build.ps1`에 반영 |
| **높음** | 리소스 경로 | `set_resource_base(SDL_GetBasePath())`는 exe 디렉터리 기준. BK/PCX 등은 보통 `data/` 등 하위에 있음 | 실행 시 리소스가 exe 옆에 있도록 배치하거나, `set_resource_base`에 `base + "data"` 등 하위 경로 지정 옵션 추가 |
| **중간** | 실제 화면 출력 없음 | NullRenderer는 `draw_surface`가 no-op. 배경 그리기 호출 경로는 있으나 픽셀 출력 없음 | OpenGL3(또는 소프트웨어) 렌더러 이식 후 `video_draw`가 실제로 그리도록 연동 |
| **중간** | 사운드 미재생 | `audio_play_sound`/`audio_play_music` API만 있고, sounds_loader·PSM/Opus 미이식, Null 백엔드 | sounds_loader·SDL 백엔드 또는 PSM/Opus 소스 이식 후 재생 연동 |
| **중간** | 설정·로그 미연동 | omf_config.json, settings_init/load, log_init 미구현 | 설정 파일 형식 정의 후 로드·엔진/비디오 옵션 연동. 로그는 Utils 이식 시 연동 |
| **낮음** | 게임 플레이 불가 | 씬 배경만 로드·렌더 경로 있고, 오브젝트·HAR·메뉴 로직·개별 씬(mainmenu, arena 등) 미이식 | object_render, har_create, scene 콜백(메뉴/아레나 등) 단계적 이식 |
| **낮음** | video_draw_remap/offset/size 미구현 | C의 video_draw_remap, video_draw_offset, video_draw_size 등 | 메뉴/콘솔 등에서 사용 시 Video 쪽에 해당 API 추가 |

### 6.4 빌드·실행 요약

| 항목 | 상태 |
|------|------|
| `.\CPP\00_build_all_modules.ps1` (Engine 제외 모듈) | 각 모듈별 스크립트 존재. Engine 제외 시 모두 성공 가정 (Engine은 링크 이슈) |
| `.\CPP\Engine\engine.build.ps1` (omf.exe) | **컴파일 성공**, **링크 실패**(SDL2 정적 + MinGW 환경) |
| 실행 파일 실행 | 링크가 성공한 환경에서만 가능. 리소스(MAIN.BK 등)는 exe 기준 경로에 필요 |
