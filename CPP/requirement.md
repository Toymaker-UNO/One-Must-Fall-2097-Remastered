# OpenOMF C++ 프로젝트 요구사항 (requirement.md)

이 문서는 **C 프로젝트를 C++로 이식**하는 CPP 디렉터리 프로젝트의 요구사항을 정의합니다.  
**레이어드 아키텍처**, **모듈/클래스 배치**, **코딩 스타일**, **파일 스타일**을 포함합니다.

---

## 1. 프로젝트 목적 및 범위

- **목적**: `C/` 디렉터리의 OpenOMF 클라이언트를 C++로 재구성한다.
- **범위**: 동일 기능 유지(비디오, 오디오, 입력, 게임 로직, 네트워크, 리소스, 포맷 등). 동작·데이터 호환은 정책에 따라 C와 공유할 수 있음.
- **제약**: 네이티브 해상도 320×200 유지, 기존 데이터 파일(BK, AF, REC 등) 호환 목표.

---

## 2. 레이어드 아키텍처

아키텍처는 **4개 레이어**로 구성한다.  
상위 레이어는 하위 레이어만 참조하며, 하위 레이어는 상위 레이어를 알지 못한다.

```
  ┌─────────────────────────────────────────────────────────┐
  │  Presentation Layer   (UI, 렌더링, 입력 장치, 콘솔)      │
  ├─────────────────────────────────────────────────────────┤
  │  Application Layer    (엔진, 씬 오케스트레이션, 메뉴 흐름) │
  ├─────────────────────────────────────────────────────────┤
  │  Domain Layer         (게임 상태, 오브젝트, 규칙, 엔티티)  │
  ├─────────────────────────────────────────────────────────┤
  │  Infrastructure Layer (파일 포맷, 리소스 로드, 플랫폼)   │
  └─────────────────────────────────────────────────────────┘
```

### 2.1 Presentation Layer (표현 계층)

- **역할**: 사용자와의 직접 상호작용. 렌더링 출력, 오디오 출력, 입력 장치 읽기, 개발자 콘솔 UI.
- **의존**: Application Layer (및 필요 시 Domain 인터페이스만).
- **모듈 및 클래스**:

| 모듈 | 설명 | 클래스 예 |
|------|------|-----------|
| **Video** | 화면 초기화, 그리기 API, 렌더러 선택 | `VideoSystem`, `Surface`, `IRenderer`, `Gl3Renderer`, `NullRenderer`, `SdlWindow` |
| **VgaState** | 전역 팔레트/리맵 상태 | `VgaState`, `VgaPalette`, `VgaRemap` |
| **Audio** | 사운드/음악 재생 API | `AudioSystem`, `IAudioBackend`, `SdlAudioBackend`, `NullAudioBackend` |
| **Input** | 키보드/조이스틱/게임패드 입력 | `Controller`, `KeyboardController`, `JoystickController`, `GameControllerDb` |
| **Console** | 개발자 콘솔 창 및 명령 UI | `Console`, `ConsoleCommand` |

---

### 2.2 Application Layer (애플리케이션 계층)

- **역할**: 게임 루프, 씬 전환, 메뉴/화면 흐름, 틱 스케줄링, 네트워크/녹화 세션 관리.
- **의존**: Domain Layer, Presentation Layer(입력/출력 인터페이스).
- **모듈 및 클래스**:

| 모듈 | 설명 | 클래스 예 |
|------|------|-----------|
| **Engine** | 초기화, 메인 루프, 종료 | `Engine`, `EngineInitFlags` |
| **SceneManagement** | 씬 생성/전환/해제, 씬 라이프사이클 | `SceneManager`, `Scene` (또는 `IScene`), `SceneFactory` |
| **GameLoop** | 정적/동적 틱, 이벤트→씬 전달 | `GameLoop`, `TickScheduler` |
| **Menus** | 메인 메뉴, 옵션, 메크랩, 로비 등 화면별 유스케이스 | `MainMenu`, `MechLabMenu`, `LobbyScreen`, `OptionsMenu` 등 (씬 구현체) |
| **NetworkSession** | ENet 호스트/피어, 로비/대전 세션 | `NetworkSession`, `LobbyClient` (또는 Application 서비스) |
| **Recording** | 녹화/재생 제어 | `RecordingSession`, `PlaybackSession` |

---

### 2.3 Domain Layer (도메인 계층)

- **역할**: 게임의 핵심 개념. 상태, 엔티티, 규칙. 플랫폼/UI 무관.
- **의존**: Infrastructure Layer(파일 포맷/리소스는 인터페이스로 주입 가능).
- **모듈 및 클래스**:

| 모듈 | 설명 | 클래스 예 |
|------|------|-----------|
| **GameState** | 전역 게임 상태, 틱, 일시정지, 다음 씬 | `GameState`, `MatchSettings` |
| **Player** | 플레이어 정보, HAR, 컨트롤러 바인딩 | `GamePlayer`, `PlayerIdentity` |
| **Object** | 월드 오브젝트 공통(위치, 속도, 애니메이션, 그룹) | `Object`, `ObjectGroup`, `CollisionResult` |
| **Combat** | HAR, 투사체, 잔해, 해저드 | `Har`, `Projectile`, `Scrap`, `Hazard` |
| **Arena** | 아레나 제약, 배경 참조 | `ArenaConstraints`, `ArenaId` |
| **Match** | 라운드, 승패, 대전 규칙 | `MatchRules`, `FightStats`, `RoundResult` |
| **SceneContract** | 씬이 구현하는 인터페이스(렌더/틱/이벤트) | `IScene` (또는 순수 가상 베이스) |

---

### 2.4 Infrastructure Layer (인프라 계층)

- **역할**: 파일 I/O, 포맷 파싱, 리소스 로드, 경로/설정, 플랫폼 래퍼.
- **의존**: 외부 라이브러리(SDL, OpenGL, enet 등). Domain/Application은 이 레이어의 추상화에만 의존.
- **모듈 및 클래스**:

| 모듈 | 설명 | 클래스 예 |
|------|------|-----------|
| **PathManager** | 리소스/설정/로그/세이브 경로 | `PathManager` |
| **ResourceIds** | 리소스 ID 열거 및 이름 매핑 | `ResourceId`, `ResourceCatalog` |
| **Formats** | BK, AF, REC, 언어, 스프라이트 등 파일 포맷 | `BkReader`, `AfLoader`, `RecFile`, `LanguageFile`, `Palette`, `VgaImage` 등 |
| **ResourceLoaders** | 게임 리소스 로드(폰트, 사운드, 언어, 토너먼트) | `BkLoader`, `SoundsLoader`, `FontLoader`, `TrnManager` |
| **Settings** | 설정 저장/로드(비디오, 오디오, 게임플레이) | `Settings`, `SettingsStore` |
| **Serialization** | 네트워크/녹화용 직렬화 | `SerialBuffer`, `SerialReader`, `SerialWriter` |
| **Platform** | 시간, 로그, 메시지박스, 크래시 핸들러 | `Logger`, `MessageBox`, `CrashHandler`, `TimeFormat` |

---

## 3. 코딩 스타일

### 3.1 명명 규칙

- **타입(클래스, 구조체, 열거형, 타입 별칭)**  
  - **PascalCase**  
  - 예: `GameState`, `VgaPalette`, `IRenderer`, `ResourceId`.

- **함수, 메서드**  
  - **camelCase** 또는 **PascalCase** (프로젝트 일관성에 따라 하나로 통일).  
  - 문서 기본값: **camelCase** (예: `getNextScene()`, `loadHar()`).

- **멤버 변수**  
  - **camelCase** 또는 **snake_case** (문서 기본값: **camelCase**, private 접미사 `_` 선택 가능).  
  - 예: `nextSceneId_`, `matchSettings`.

- **지역 변수, 매개변수**  
  - **camelCase** (또는 snake_case로 통일).  
  - 예: `sceneId`, `playerIndex`.

- **상수, 열거자, 매크로**  
  - **UPPER_SNAKE_CASE** 또는 **kPascalCase** (문서 기본값: **UPPER_SNAKE_CASE** for macros, **kPascalCase** for constexpr).  
  - 예: `MAX_PLAYERS`, `kNativeWidth`.

- **네임스페이스**  
  - **snake_case** 또는 **PascalCase** (문서 기본값: **snake_case**).  
  - 예: `openomf::video`, `openomf::domain`.

### 3.2 포맷팅

- **들여쓰기**: 스페이스 **4칸**.
- **한 줄 길이**: **120자** 이내 권장 (넘을 경우 줄바꿈, 정렬 스타일 팀 합의).
- **중괄호**: K&R 스타일. 함수/클래스/제어문 `{` 다음 줄 또는 같은 줄 팀 합의 후 통일.
- **줄 끝**: LF (Unix style).

### 3.3 C++ 관례

- **const**: 변경 불가 인자·멤버·반환은 `const` 사용. getter는 `const` 메서드.
- **스마트 포인터**: 소유권이 있을 경우 `std::unique_ptr` / `std::shared_ptr` 사용.裸 포인터는 “참조만” 할 때.
- **RAII**: 리소스(파일, 핸들, GL 객체 등)는 생성자에서 획득, 소멸자에서 해제.
- **override / final**: 가상 함수 재정의 시 `override`, 더 이상 파생 금지 시 `final` 명시.
- **헤더**: 필요한 최소 include. 전방 선언으로 해결 가능하면 include 대체.
- **네임스페이스**: 프로젝트 루트 네임스페이스 예: `openomf`. 레이어/모듈별 하위 네임스페이스 사용 권장.

### 3.4 금지·자제

- **매크로**: 상수/인라인 대체 가능하면 매크로 자제. 매크로 사용 시 UPPER_SNAKE_CASE.
- **전역 가변 상태**: 가능한 한 클래스/DI로 캡슐화.
- **using namespace std**: 헤더에서는 사용 금지. 구현 파일에서도 좁은 스코프 권장.

---

## 4. 파일 스타일

### 4.1 확장자

- **헤더**: `.hpp` (C++ 전용).  
  - C 호환 헤더가 필요한 경우만 `.h` 및 `extern "C"` 검토.
- **구현**: `.cpp`.

### 4.2 파일 명명

- **한 파일에 한 클래스**를 두는 것을 기본으로 한다.  
  - 파일명 = 클래스명과 동일 (PascalCase).  
  - 예: `GameState.hpp` / `GameState.cpp`, `IRenderer.hpp`.
- **인터페이스(순수 가상)**  
  - 접두사 `I` (예: `IRenderer.hpp`, `IAudioBackend.hpp`).
- **작은 타입/유틸**  
  - 여러 개를 한 헤더에 모을 수 있음. 이때 파일명은 모듈/역할에 맞게 (예: `Types.hpp`, `Enums.hpp`).

### 4.3 디렉터리 구조 (레이어·모듈 반영)

```
CPP/
├── requirement.md
├── src/
│   ├── presentation/     # Presentation Layer
│   │   ├── video/
│   │   ├── vga/
│   │   ├── audio/
│   │   ├── input/
│   │   └── console/
│   ├── application/     # Application Layer
│   │   ├── engine/
│   │   ├── scene/
│   │   ├── gameloop/
│   │   ├── menus/
│   │   ├── network/
│   │   └── recording/
│   ├── domain/           # Domain Layer
│   │   ├── state/
│   │   ├── player/
│   │   ├── object/
│   │   ├── combat/
│   │   ├── arena/
│   │   └── match/
│   └── infrastructure/   # Infrastructure Layer
│       ├── path/
│       ├── resource/
│       ├── formats/
│       ├── settings/
│       ├── serialization/
│       └── platform/
├── include/              # (선택) 공개 API만 놓을 경우
└── resources/            # 셰이더, 데이터 등 (C와 공유 또는 링크)
```

- **include 경로**: `#include` 시 레이어/모듈 기준 상대 경로 또는 프로젝트 루트 기준 경로 중 하나로 통일.  
  - 예: `#include "application/engine/Engine.hpp"` 또는 `#include "engine/Engine.hpp"` (include path에 `src` 또는 `src/application` 포함).

### 4.4 헤더 가드

- **Include guard** 사용: `#ifndef OPENOMF_모듈_파일명_HPP` / `#define` / `#endif`  
  - 또는 **#pragma once** (사용 시 프로젝트 전역 통일).
- 매크로 이름: **UPPER_SNAKE_CASE**, 프로젝트 접두사 포함 (예: `OPENOMF_ENGINE_HPP`).

### 4.5 문서화

- **공개 API**: Doxygen 스타일 주석 권장 (`///`, `@param`, `@return`, `@brief`).
- **복잡한 비즈니스 로직**: 한글/영어 주석으로 의도 명시.

---

## 5. C → CPP 매핑 참고

| C (대상) | CPP 레이어 | CPP 모듈/클래스 참고 |
|----------|------------|----------------------|
| video.c/h, surface, renderers | Presentation | Video, Surface, IRenderer, Gl3Renderer |
| vga_state, vga_palette, vga_remap | Presentation | VgaState, VgaPalette, VgaRemap |
| audio.c/h, backends, sources | Presentation | AudioSystem, IAudioBackend, *Source |
| controller*, keyboard, joystick, net_controller | Presentation / Application | Input (Controller, Keyboard, …), Application (NetworkSession) |
| console | Presentation | Console |
| engine.c/h | Application | Engine |
| game_state, scene (protos) | Application + Domain | SceneManager, Scene, GameState (Domain) |
| object, har, projectile, scrap, hazard | Domain | Object, Har, Projectile, Scrap, Hazard |
| game_player, match_settings | Domain | GamePlayer, MatchSettings |
| gui/* | Presentation | Gui (Widget, Button, Menu, …) |
| formats/* | Infrastructure | Formats (BkReader, AfLoader, …) |
| pathmanager, ids, *loader, *manager | Infrastructure | PathManager, ResourceIds, ResourceLoaders |
| settings | Infrastructure | Settings |
| serial | Infrastructure | Serialization |
| utils (log, msgbox, time, …) | Infrastructure | Platform |

---

## 6. 문서 갱신

- 레이어/모듈/클래스 추가·변경 시 이 문서를 갱신한다.
- 코딩/파일 스타일 예외는 팀 합의 후 이 문서에 명시한다.

---

*작성 후 추가 편집은 프로젝트 담당자가 수행합니다.*
