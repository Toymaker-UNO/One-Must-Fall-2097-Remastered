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
- **다음 진행**: Formats 빌드·테스트로 컴파일/링크 검증.

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
