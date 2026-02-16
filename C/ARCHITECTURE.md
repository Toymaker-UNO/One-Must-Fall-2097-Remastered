# One Must Fall 2097 Remastered (OpenOMF) — C 프로젝트 아키텍처

이 문서는 `C/` 디렉터리 아래의 **모든 파일**을 기준으로 한 아키텍처·구조·모듈 정리입니다.

---

## 1. 프로젝트 개요

- **목적**: One Must Fall 2097 리메이크/리마스터 클라이언트 (OpenOMF)
- **언어**: C (C11), 빌드 스크립트는 PowerShell
- **출력**: `build/openomf.exe` (Windows), vcpkg 기반 의존성
- **네이티브 해상도**: 320×200 (VGA 레거시 유지)

---

## 2. 디렉터리 및 파일 구조 (C/ 전체)

```
C/
├── 00_build.ps1                 # 단일 스크립트 빌드 (gcc/g++, vcpkg lib 사용)
├── ARCHITECTURE.md               # 본 문서
├── lib/                          # vcpkg 등 외부 라이브러리 (include/bin)
│   └── include/                  # SDL2, enet, libpng, epoxy, opus, vorbis, confuse 등
├── resources/                    # 런타임/배포 리소스
│   ├── shaders/                  # GLSL 셰이더 (palette, rgba)
│   ├── game_resources/          # 게임 내장 리소스 (PCX, gamecontrollerdb 등)
│   ├── gamecontrollerdb/         # SDL 게임패드 DB
│   ├── icons/                    # 아이콘, openomf.rc
│   ├── flatpak/                  # Flatpak 메타정보
│   ├── *.TXT                     # 언어 파일 (ENGLISH2, DANISH, GERMAN2 등)
│   ├── openomf.gpl               # GPL 라이선스
│   └── create_openomf_bk.sh      # BK 리소스 생성 스크립트
└── src/                          # 소스 코드 전부
    ├── main.c                    # 진입점, CLI(argtable3), pm_init, engine_init/run
    ├── engine.c / engine.h       # 엔진 초기화·메인 루프
    │
    ├── video/                    # 비디오 서브시스템
    │   ├── video.c,h             # 공개 API (video_init, video_draw_*, video_render_*)
    │   ├── surface.c,h          # 픽셀 버퍼 (w,h,data, transparency)
    │   ├── image.c,h             # 이미지 유틸
    │   ├── color.h
    │   ├── enums.h
    │   ├── vga_state.c,h         # 전역 VGA 상태 (팔레트, 리맵 등)
    │   ├── vga_palette.c,h
    │   ├── vga_remap.c,h
    │   ├── damage_tracker.c,h
    │   └── renderers/
    │       ├── renderer.h        # 렌더러 인터페이스 (함수 포인터 테이블)
    │       ├── common.c,h        # 공통 로직
    │       ├── null/             # null_renderer (테스트/헤드리스)
    │       └── opengl3/          # OpenGL 3 렌더러
    │           ├── gl3_renderer.c,h
    │           ├── sdl_window.c,h
    │           └── helpers/      # texture, texture_atlas, vbo, vao, ubo, fbo, shaders, remaps, render_target, bindings, object_array, shared
    │
    ├── audio/                    # 오디오 서브시스템
    │   ├── audio.c,h             # 공개 API (audio_init, audio_play_sound/music 등)
    │   ├── backends/
    │   │   ├── audio_backend.h   # 백엔드 인터페이스
    │   │   ├── sdl/sdl_backend.c,h
    │   │   └── null/null_backend.c,h
    │   └── sources/
    │       ├── music_source.h
    │       ├── psm_source.c,h    # PSM 모듈
    │       └── opus_source.c,h   # Opus
    │
    ├── controller/               # 입력/네트워크 제어
    │   ├── controller.c,h        # controller_t, ctrl_event, tick/poll/rumble/rewind
    │   ├── keyboard.c,h
    │   ├── joystick.c,h
    │   ├── game_controller_db.c,h
    │   ├── ai_controller.c,h
    │   ├── net_controller.c,h    # ENet 기반 네트 플레이
    │   ├── rec_controller.c,h   # 녹화/재생
    │   └── spec_controller.c,h   # 관전
    │
    ├── console/                  # 개발자 콘솔
    │   ├── console.c,h           # 콘솔 열기/닫기, 이벤트, 렌더, 명령 등록
    │   ├── console_cmd.c         # 명령 구현
    │   └── console_type.h
    │
    ├── game/                     # 게임 로직 전반
    │   ├── game_state.c,h        # 게임 상태 (씬, 오브젝트, 플레이어, 틱, 네트/녹화)
    │   ├── game_state_type.h     # game_state_t, match_settings, 레이어/역할/네트모드
    │   ├── game_player.c,h       # 플레이어 (HAR, 컨트롤러, 이름 등)
    │   ├── common_defines.c,h    # 씬/난이도/HAR/파일럿/라운드 ID, 이름 매핑
    │   │
    │   ├── protos/               # “프로토타입” 게임 엔티티
    │   │   ├── scene.c,h         # scene_t, 씬 콜백(render/tick/event/startup 등), scene_create/load_har
    │   │   ├── object.c,h        # object_t (위치, 속도, 애니메이션, 그룹, 콜백)
    │   │   ├── player.c,h        # player_sprite_state, player_animation_state 등
    │   │   ├── intersect.c,h     # 충돌/교차
    │   │   └── fight_stats.h
    │   │
    │   ├── objects/              # 게임 오브젝트 구현체
    │   │   ├── har.c,h           # HAR (로봇)
    │   │   ├── projectile.c,h
    │   │   ├── scrap.c,h
    │   │   ├── hazard.c,h
    │   │   └── arena_constraints.h
    │   │
    │   ├── gui/                  # 메뉴/UI 위젯
    │   │   ├── gui.h             # 통합 include
    │   │   ├── component.c,h, widget.c,h, gui_frame.c,h
    │   │   ├── button.c,h, label.c,h, gauge.c,h, progressbar.c,h
    │   │   ├── menu.c,h, menu_background.c,h
    │   │   ├── dialog.c,h, filler.c,h
    │   │   ├── sizer.c,h, xysizer.c,h
    │   │   ├── textinput.c,h, textselector.c,h, textslider.c,h
    │   │   ├── portrait.c,h, spritebutton.c,h, spriteimage.c,h
    │   │   ├── trn_menu.c,h, trnselect.c,h
    │   │   ├── theme.h
    │   │   └── text/
    │   │       ├── text.c,h, text_layout.c,h, enums.h
    │   │
    │   ├── scenes/               # 씬 구현 (씬 ID별 화면)
    │   │   ├── intro.c,h
    │   │   ├── openomf.c,h
    │   │   ├── mainmenu.c,h + mainmenu/ (menu_main, menu_audio, menu_video, menu_connect, menu_net, menu_netconfig, menu_gameplay, menu_advanced, menu_help, menu_language, menu_input, menu_keyboard, menu_configuration, menu_presskey, menu_listen, menu_video_confirm, menu_widget_ids)
    │   │   ├── mechlab.c,h + mechlab/ (lab_dash_*, lab_menu_*, button_details, lab_har_constants)
    │   │   ├── vs.c,h, melee.c, arena.c
    │   │   ├── newsroom.c,h, scoreboard.c,h
    │   │   ├── credits.c,h, cutscene.c,h
    │   │   ├── lobby.c,h
    │   │   └── end/end1/end2 (SCENE_END*)
    │   │
    │   └── utils/                # 게임 전용 유틸
    │       ├── settings.c,h      # 설정 (비디오, 사운드, 게임플레이, 토너먼트 등)
    │       ├── version.c,h
    │       ├── ticktimer.c,h
    │       ├── serial.c,h        # 직렬화 (네트/녹화/롤백)
    │       ├── score.c,h, formatting.c,h
    │       ├── har_screencap.c,h
    │       └── nat.c,h
    │
    ├── formats/                  # 파일 포맷 (로드/저장/구조체)
    │   ├── internal/
    │   │   ├── reader.c,h, writer.c,h
    │   │   ├── memreader.c,h, memwriter.c,h
    │   │
    │   ├── palette.c,h, vga_image.c,h, rgba_image.c
    │   ├── pcx.c,h
    │   ├── bk.c,h, bkanim.c,h    # 배경/애니메이션
    │   ├── af.c,h                # HAR 동작 데이터
    │   ├── animation.c,h, script.c,h, taglist.c,h, taglist_helpers.c
    │   ├── move.c,h, actions.h
    │   ├── sprite.c,h, pic.c,h
    │   ├── chr.c,h, colcoord.h
    │   ├── fonts.c,h
    │   ├── sounds.c,h
    │   ├── pilot.c,h, score.c,h
    │   ├── setup.c,h
    │   ├── language.c,h
    │   ├── rec.c,h, rec_assertion.c,h   # 녹화/재생
    │   ├── tournament.c,h
    │   ├── altpal.c,h
    │   └── error.c,h
    │
    ├── resources/                # 리소스 로더·매니저 (게임 데이터 바인딩)
    │   ├── pathmanager.c,h       # 경로 (RESOURCE_PATH, LOG_PATH, CONFIG_PATH 등)
    │   ├── ids.c,h               # resource_id (BK_*, AF_*, PSM_*, DAT_*, PCX_*)
    │   ├── bk_loader.c,h, bk.c,h, bk_info.c,h
    │   ├── af_loader.c,h, af_move.c,h
    │   ├── sprite.c,h, animation.c,h
    │   ├── fonts.c,h
    │   ├── languages.c,h
    │   ├── sounds_loader.c,h
    │   ├── scores.c,h
    │   ├── pilots.c,h
    │   ├── sgmanager.c,h         # 스프라이트/그래픽 매니저
    │   └── trnmanager.c,h        # 토너먼트 매니저
    │
    ├── utils/                    # 범용 유틸리티
    │   ├── allocator.c,h, allocator_default.h
    │   ├── array.c,h, vector.c,h, list.c,h, iterator.c,h
    │   ├── vec.h, hashmap.c,h
    │   ├── str.c,h, c_string_util.c,h, c_array_util.h
    │   ├── log.c,h, msgbox.c,h
    │   ├── config.c,h, io.c,h
    │   ├── random.c,h, miscmath.h
    │   ├── time_fmt.c,h
    │   ├── png_reader.c,h, png_writer.c,h
    │   ├── scandir.c,h
    │   ├── ringbuffer.c,h
    │   ├── compat.h, cp437.c,h
    │   └── crash.c,h
    │
    └── vendored/
        ├── argtable3.c,h         # CLI 인자 파싱
        └── LICENSE.argtable3
```

---

## 3. 아키텍처 개요

### 3.1 진입점 및 초기화 순서

1. **main.c**
   - `pm_init()` → 경로/리소스 검증
   - CLI 파싱 (argtable3): `--help`, `--version`, `--listen`, `--lobby`, `--connect`, `--record`, `--playback`, 강제 렌더러/오디오 백엔드 등
   - `engine_init(init_flags)` 호출
   - `engine_run(init_flags)` 호출
   - 종료 시 `engine_close()`, `pm_free()` 등

2. **engine_init()** (engine.c)
   - `video_scan_renderers()` / `audio_scan_backends()`
   - `video_init()` → 렌더러 선택 및 창/컨텍스트 생성
   - `audio_init()` → 샘플레이트, 볼륨, 백엔드
   - `sounds_loader_init()` → `DAT_SOUNDS` 등
   - `lang_init()` → 언어
   - `fonts_init()` → 폰트
   - `altpals_init()` → 대체 팔레트
   - `console_init()`
   - `vga_state_init()`

### 3.2 메인 루프 (engine_run)

- **이벤트**: `SDL_PollEvent` → 콘솔 열림 시 `console_event`, 아니면 `game_state_handle_event`
- **틱**: 정적 틱(10ms) / 동적 틱(설정 가능) 제한 (`MAX_TICKS_PER_FRAME`, `TICK_EXPIRY_MS`)
  - `game_state_static_tick()` / `game_state_dynamic_tick()`
  - `game_state_tick_controllers()`
- **렌더**: `game_state_render()` → 현재 씬의 `scene_render` / `scene_render_overlay`
- **프레임 대기**: vsync/프레임 제한에 맞춰 대기

---

## 4. 핵심 모듈 상세

### 4.1 Engine (engine.c/h)

- **역할**: 창/오디오/리소스 초기화, 메인 루프, 스크린샷/팔레트 저장, REC 저장
- **의존**: video, audio, game_state, console, controller, formats(rec, altpal), resources(lang, fonts, sounds_loader), utils

### 4.2 Video

- **video.c/h**: 공개 API. `video_init`, `video_draw`, `video_draw_remap`, `video_draw_offset`, `video_draw_size`, `video_draw_full`, `video_render_prepare/finish`, `video_render_area_*`, `video_schedule_screenshot`, `video_draw_atlas`
- **surface**: 픽셀 버퍼 (w, h, data, transparent). VGA 이미지·스프라이트 렌더링 단위
- **vga_state**: 전역 팔레트/리맵 상태 (디버그 스크린샷 등)
- **renderers/renderer.h**: 렌더러 인터페이스 (설정, draw_surface, render_prepare/finish, capture_screen 등)
- **opengl3**: SDL 창 + OpenGL 3, 텍스처 아틀라스, VBO/VAO/UBO, FBO, 셰이더(palette/rgba), 리맵/렌더 타깃
- **null**: 구현만 있는 널 렌더러

### 4.3 Audio

- **audio.c/h**: `audio_init`, `audio_play_sound` / `audio_play_sound_buf`, `audio_fade_out`, `audio_play_music`, `audio_stop_music`, 볼륨 설정
- **backends/audio_backend.h**: 백엔드 인터페이스 (설정, play_sound, play_music, stop_music, fade_out)
- **sdl**: SDL2_mixer 기반
- **null**: 널 백엔드
- **sources**: PSM, Opus 등 음원 타입

### 4.4 Controller

- **controller_t**: game_state, har_obj_id, tick_fun, poll_fun, rumble_fun, har_hook, rewind_fun, free_fun, type(keyboard/gamepad/network/ai/rec/spectator)
- **ctrl_event**: type(action, HB, propose_start, confirm_start, game_info, close), 링크드 리스트
- 키보드, 조이스틱, 게임패드 DB, AI, ENet 네트워크, 녹화/재생, 관전 전용 컨트롤러

### 4.5 Game State & Scene

- **game_state_t**: run, paused, tick, scene(sc), objects(vector), players[2], match_settings, net_mode, rec, menu_ctrl, hit_pause, screen_shake 등
- **scene_t**: game_state, bk_data, af_data[2], 콜백 (free, event, render, render_overlay, static_tick, dynamic_tick, input_poll, startup, prio_override, clone, clone_free)
- **씬 전환**: `game_state_set_next(gs, next_scene_id)` → 다음 프레임에 씬 생성/로드
- **씬 ID**: common_defines (SCENE_INTRO, SCENE_MENU, SCENE_MECHLAB, SCENE_VS, SCENE_MELEE, SCENE_ARENA0~4, SCENE_LOBBY 등)

### 4.6 Object (프로토타입 + 구현체)

- **object_t**: id, pos, vel, direction, group(HAR/SCRAP/PROJECTILE/HAZARD 등), 애니메이션/스프라이트, 레이어, 콜백(act, static_tick, dynamic_tick, collide, move, finish, clone)
- **protos/object**: 공통 생성/틱/렌더/충돌/직렬화
- **objects/har**: 플레이어 기체
- **objects/projectile, scrap, hazard**: 전투/아레나 오브젝트

### 4.7 GUI

- **구성**: component, widget, gui_frame, button, label, gauge, progressbar, menu, dialog, sizer, textinput, textselector, textslider, portrait, spritebutton, theme, text 레이아웃
- **용도**: 메인 메뉴, 메크랩, 옵션, 네트워크 설정, 토너먼트 선택 등

### 4.8 Formats

- **역할**: 원작/리마스터 데이터 파일 포맷 정의 및 로드/저장
- **주요**: bk/bkanim(배경), af(동작), animation/script/taglist(애니메이션), move/actions(입력), sprite/pic/vga_image(그래픽), chr/colcoord(캐릭터/충돌), fonts, sounds, pilot, score, setup, language, rec/rec_assertion(녹화), tournament, altpal
- **internal**: reader/writer, memreader/memwriter (스트림/메모리 입출력)

### 4.9 Resources

- **pathmanager**: 리소스/로그/설정/스코어/세이브/셰이더 경로, 휴대/릴리즈/디버그 모드
- **ids**: resource_id → 파일명/이름, is_arena/is_har/is_music
- **bk_loader, af_loader, sounds_loader**: formats와 연동해 게임 리소스 로드
- **sgmanager, trnmanager, fonts, languages, pilots, scores, bk_info**: 특정 도메인 매니저/캐시

### 4.10 Utils

- **메모리**: allocator, array, vector, list, iterator, hashmap
- **문자열/배열**: str, c_string_util, c_array_util, vec
- **시스템**: log, msgbox, config, io, scandir, compat, cp437, crash
- **수학/시간**: random, miscmath, time_fmt
- **미디어**: png_reader, png_writer
- **기타**: ringbuffer

### 4.11 Console

- **역할**: 개발자 콘솔 (명령 등록/실행, 출력, 이벤트/틱)
- **연동**: game_state 인자로 명령 실행

### 4.12 Vendored

- **argtable3**: main.c CLI 옵션 파싱

---

## 5. 데이터 흐름 요약

- **입력**: SDL 이벤트 → 콘솔 또는 game_state → scene → controller → object(act 등)
- **틱**: engine_run → game_state_static_tick / game_state_dynamic_tick → scene → object static_tick/dynamic_tick, game_state_tick_controllers
- **렌더**: game_state_render → scene_render/render_overlay → video_draw_* (surface 기반) → 현재 렌더러(draw_surface 등)
- **리소스**: pathmanager + ids → formats(파일 파싱) → resources(로더/매니저) → game_state / scene / object / gui
- **네트/녹화**: serial + rec 포맷, net_controller/rec_controller, game_state_clone/rewind/replay

---

## 6. 빌드 시스템

- **스크립트**: `C/00_build.ps1` (PowerShell)
- **동작**: `src` 아래 모든 `.c` (및 필요 시 `.cpp`) 수집 → gcc/g++로 `-I src, src/vendored, lib/include` 등으로 컴파일 → build/ 에 `.o` 생성 → 링커로 `openomf.exe` 생성
- **플래그 예**: C11, SDL_MAIN_HANDLED, ENABLE_SDL_AUDIO_BACKEND, ENABLE_OPENGL3_RENDERER, ENABLE_NULL_*, USE_LIBPNG, USE_OPUSFILE, LIBXMP_STATIC 등
- **링크**: SDL2, SDL2_mixer, xmp, epoxy, enet, confuse, png, zlib, opusfile, opus, vorbis*, ogg, winmm, ws2_32, ole32 등 (Windows)
- **사후 처리**: `resources/game_resources`, `resources/shaders` 를 build/resources, build/shaders 로 복사

---

## 7. 외부 라이브러리 (lib/)

- **SDL2**: 창, 이벤트, 입력, OpenGL 컨텍스트
- **SDL2_mixer**: 사운드 재생
- **epoxy**: OpenGL 로딩
- **libpng, zlib**: PNG/이미지
- **opus, opusfile, vorbis, ogg**: 오디오 코덱
- **xmp**: 모듈 음악 (PSM 등)
- **enet**: 네트워크 (로비/대전)
- **confuse**: 설정 파일 파싱

---

## 8. 리소스 파일 (resources/)

| 경로 | 용도 |
|------|------|
| shaders/*.vert, *.frag | OpenGL 셰이더 (palette, rgba) |
| game_resources/*.PCX, gamecontrollerdb.txt | 내장 그래픽/패드 DB |
| gamecontrollerdb/ | SDL 게임패드 매핑 |
| *.TXT | 언어 문자열 |
| flatpak/* | Flatpak 패키징 |
| openomf.gpl | 라이선스 |

---

이 문서는 `C/` 트리와 소스 분석을 바탕으로 작성되었으며, 추후 파일 추가/삭제 시 함께 갱신하는 것을 권장합니다.
