# OpenOMF C++ 프로젝트 요구사항 (requirement.md)

이 문서는 **C 프로젝트를 C++로 이식**하는 CPP 디렉터리 프로젝트의 요구사항을 정의합니다.  
**레이어드 아키텍처**, **모듈/클래스 배치**, **코딩 스타일**, **파일 스타일**을 포함합니다.

---

## 1. 프로젝트 목적 및 범위

- **목적**: `C/` 디렉터리의 OpenOMF 클라이언트를 C++로 재구성한다.
- **범위**: 동일 기능 유지(비디오, 오디오, 입력, 게임 로직, 네트워크, 리소스, 포맷 등). 동작·데이터 호환은 정책에 따라 C와 공유할 수 있음.
- **제약**: 네이티브 해상도 320×200 유지, 기존 데이터 파일(BK, AF, REC 등) 호환 목표.
- **타겟 플랫폼**: Windows, Linux, Android, iOS, Nintendo Switch, PlayStation, Xbox (멀티플랫폼 목표).

---

## 2. 본과제 요구사항

본 프로젝트는 아래 요구사항을 만족하도록 설계·구현한다. 아키텍처(§3) 및 콘텐츠 확장·파이프라인 확장(§3 내 해당 절)은 이 요구사항에 부합하도록 기술하였다.

### 2.1 콘텐츠 확장성

| 요구사항 ID | 내용 | 비고 |
|-------------|------|------|
| **REQ-CONTENT-01** | **로봇(캐릭터)의 추가/제거가 쉬워야 한다.** | 데이터·매니페스트 위주 변경으로 추가/제거 가능. 코드 변경 최소화(§3.8.1, §3.8.4 참고). |
| **REQ-CONTENT-02** | **배경 화면(전투 배경·스테이지)의 추가/제거가 쉬워야 한다.** | 동일하게 데이터·매니페스트 중심(§3.8.2, §3.8.4 참고). |

### 2.2 하드웨어·플랫폼 확장성

| 요구사항 ID | 내용 | 비고 |
|-------------|------|------|
| **REQ-HW-01** | **Windows, Linux, Nintendo Switch, Android, iOS, PlayStation, Xbox** 등 서로 다른 H/W·플랫폼으로의 확장성이 좋아야 한다. | PAL(OS 추상화)·HAL(하드웨어 추상화) 분리로, 새 플랫폼 추가 시 해당 레이어 구현체만 추가(§3.0, §3.5, §3.6 참고). |

### 2.3 테스트 용이성

| 요구사항 ID | 내용 | 비고 |
|-------------|------|------|
| **REQ-TEST-01** | **테스트가 쉬워야 한다.** | PAL·HAL의 null 구현(헤드리스·모의 입출력), Domain/Application의 플랫폼 무관 설계, 콘텐츠 백엔드·렌더 백엔드 교체 가능 구조로 단위·통합·자동화 테스트가 용이해야 한다. |

### 2.4 다른 엔진으로의 확장성·이동성

| 요구사항 ID | 내용 | 비고 |
|-------------|------|------|
| **REQ-ENGINE-01** | **Unity, Unreal** 등 다른 엔진으로의 확장성·이동성이 좋아야 한다. | 중립 런타임 모델(NeutralCharacter, NeutralAnimation, NeutralStage) 및 콘텐츠 백엔드(Unity/Unreal export) 도입. 동일 게임 로직을 유지한 채 해당 엔진에서 제작한 애니메이션·모델을 사용하거나, 필요 시 엔진 간 에셋/로직 이전이 가능하도록 설계(§3.9 참고). |

---

## 3. 레이어드 아키텍처

아키텍처는 **7개 레이어**로 구성한다.  
상위 레이어는 하위 레이어만 참조하며, 하위 레이어는 상위 레이어를 알지 못한다.  
**Common** 레이어는 모든 레이어에서 사용할 수 있다.  
플랫폼·하드웨어 차이는 **PAL**(Platform Abstraction Layer)과 **HAL**(Hardware Abstraction Layer) 두 계층에서만 다루며, 이 두 레이어의 구현체에만 플랫폼별 코드를 둔다.

```
  ┌─────────────────────────────────────────────────────────┐
  │  Presentation  (표현)   — 사용자 I/O: 렌더·오디오·입력·GUI     │
  ├─────────────────────────────────────────────────────────┤
  │  Application   (응용)   — 유스케이스·오케스트레이션·씬·루프     │
  ├─────────────────────────────────────────────────────────┤
  │  Domain        (도메인)  — 엔티티·상태·비즈니스 규칙 (게임 로직)  │
  ├─────────────────────────────────────────────────────────┤
  │  Platform      (데이터) — 파일·리소스·경로·설정 (게임 데이터 I/O) │
  ├─────────────────────────────────────────────────────────┤
  │  HAL           (하드웨어) — GPU·오디오·입력 장치 추상화         │
  ├─────────────────────────────────────────────────────────┤
  │  PAL           (플랫폼)  — OS·파일·창·스레드·시스템 UI 추상화   │
  ├─────────────────────────────────────────────────────────┤
  │  Common        (공통)   — 유틸·타입·도구 (플랫폼 무관)          │
  └─────────────────────────────────────────────────────────┘
```

### 3.0 멀티플랫폼을 위한 원칙

- **PAL·HAL만 플랫폼별 분기**: `#ifdef`, 플랫폼 SDK(Windows API, Android NDK, iOS, Switch SDK, PlayStation SDK) 사용, 플랫폼별 빌드 설정은 **PAL**·**HAL** 구현체 안에서만 허용한다. Presentation / Application / Domain / Platform / Common은 플랫폼을 전제하지 않고 작성한다.
- **PAL (Platform Abstraction Layer)**: **OS·플랫폼** 차이 흡수. 파일 시스템, 경로, 창(OS 관점), 스레드, 프로세스/환경, 로그, 메시지박스, 크래시 리포팅, 시간. Platform 레이어는 PAL 인터페이스만 사용한다.
- **HAL (Hardware Abstraction Layer)**: **하드웨어·디바이스** 차이 흡수. 그래픽(GPU), 오디오(사운드 디바이스), 입력(키보드·조이스틱·터치). Presentation 레이어는 HAL 인터페이스만 사용한다. HAL은 필요 시 PAL의 창·경로 등을 이용해 디바이스를 생성한다(예: PAL의 IWindow로 OpenGL 컨텍스트 생성).
- **인터페이스로 접근**: Presentation은 HAL의 `IRenderDevice`, `IAudioDevice`, `IInputProvider` 등만, Platform은 PAL의 `IFileSystem`, `IPathProvider`, `ILogger`, `IMessageBox` 등만 사용한다.
- **새 플랫폼 추가**: PAL에 해당 OS용 구현, HAL에 해당 플랫폼용 그래픽/오디오/입력 구현을 추가한다. 나머지 레이어는 수정하지 않는다.
- **크로스 플랫폼 라이브러리**: SDL, OpenGL ES, Vulkan, FMOD 등은 **PAL·HAL 구현체에서만** 사용한다. 상위 레이어는 이들 라이브러리를 직접 include하지 않는다.

| 레이어 | 멀티플랫폼 관점 |
|--------|------------------|
| **Presentation** | HAL의 렌더/오디오/입력 인터페이스만 사용. 플랫폼 무관. |
| **Application** | 플랫폼 무관. |
| **Domain** | 플랫폼 무관. |
| **Platform** | PAL의 파일/경로/로그/메시지박스 인터페이스만 사용. 플랫폼 무관. |
| **HAL** | **하드웨어별 구현** (GPU, 오디오, 입력). PAL의 창·경로 사용 가능. 플랫폼별 구현. |
| **PAL** | **OS·플랫폼별 구현** (win32, linux, android, ios, switch, playstation, xbox). 인터페이스는 공통. |
| **Common** | 플랫폼 무관 (표준 라이브러리만). |

### 3.1 Presentation (표현)

- **역할**: **Presentation Layer**. 사용자 I/O — 렌더링, 오디오 출력, 입력 장치, GUI·콘솔. **HAL이 제공하는 인터페이스**(`IRenderDevice`, `IAudioDevice`, `IInputProvider` 등)만 사용하며, GPU·오디오·입력 관련 플랫폼 API는 직접 사용하지 않는다.
- **의존**: Application 레이어 (및 필요 시 Domain 인터페이스만). HAL 레이어(인터페이스만). Common 사용 가능.
- **모듈 및 클래스**:

| 모듈 | 설명 | 클래스 예 |
|------|------|-----------|
| **Video** | 화면 초기화, 그리기 API, 렌더러 선택 | `VideoSystem`, `Surface`, `IRenderer`, `Gl3Renderer`, `NullRenderer`, `SdlWindow` |
| **VgaState** | 전역 팔레트/리맵 상태 | `VgaState`, `VgaPalette`, `VgaRemap` |
| **Audio** | 사운드/음악 재생 API | `AudioSystem`, `IAudioBackend`, `SdlAudioBackend`, `NullAudioBackend` |
| **Input** | 키보드/조이스틱/게임패드 입력 | `Controller`, `KeyboardController`, `JoystickController`, `GameControllerDb` |
| **Console** | 개발자 콘솔 창 및 명령 UI | `Console`, `ConsoleCommand` |
| **Gui** | 메뉴·다이얼로그·위젯 등 UI 요소 | `Widget`, `Button`, `Label`, `Menu`, `Dialog` |

---

### 3.2 Application (응용)

- **역할**: **Application Layer**. 유스케이스·오케스트레이션. 엔진, 씬 전환, 메인 루프, 메뉴/로비 화면, 네트워크·녹화 세션.
- **의존**: Domain 레이어, Presentation 레이어(입력/출력 인터페이스). Common 사용 가능.
- **모듈 및 클래스**:

| 모듈 | 설명 | 클래스 예 |
|------|------|-----------|
| **Engine** | 초기화, 메인 루프, 종료 | `Engine`, `EngineInitFlags` |
| **SceneManagement** | 씬 생성/전환/해제, 씬 라이프사이클 | `SceneManager`, `IScene`, `SceneFactory` |
| **GameLoop** | 정적/동적 틱, 이벤트→씬 전달 | `GameLoop`, `TickScheduler` |
| **Menus** | 메인 메뉴, 옵션, 메크랩, 로비 등 화면별 유스케이스 | `MainMenu`, `MechLabMenu`, `LobbyScreen`, `OptionsMenu` 등 (씬 구현체) |
| **NetworkSession** | ENet 호스트/피어, 로비/대전 세션 | `NetworkSession`, `LobbyClient` |
| **Recording** | 녹화/재생 제어 | `RecordingSession`, `PlaybackSession` |

---

### 3.3 Domain (도메인)

- **역할**: **Domain Layer**. 엔티티, 집합체, 비즈니스 규칙. 플랫폼/화면과 무관한 순수 게임 로직.
- **의존**: Platform 레이어(파일·리소스는 인터페이스로 주입). Common 사용 가능.
- **모듈 및 클래스**:

| 모듈 | 설명 | 클래스 예 |
|------|------|-----------|
| **GameState** | 전역 게임 상태, 틱, 일시정지, 다음 씬 | `GameState`, `MatchSettings` |
| **Player** | 플레이어 정보, HAR, 컨트롤러 바인딩 | `GamePlayer`, `PlayerIdentity` |
| **Object** | 월드 오브젝트 공통(위치, 속도, 애니메이션, 그룹) | `Object`, `ObjectGroup`, `CollisionResult` |
| **Combat** | HAR, 투사체, 잔해, 해저드 | `Har`, `Projectile`, `Scrap`, `Hazard` |
| **Arena** | 아레나 제약, 배경 참조 | `ArenaConstraints`, `ArenaId` |
| **Match** | 라운드, 승패, 대전 규칙 | `MatchRules`, `FightStats`, `RoundResult` |
| **SceneContract** | 씬이 구현하는 인터페이스(렌더/틱/이벤트) | `IScene` (순수 가상 베이스) |

---

### 3.4 Platform (데이터)

- **역할**: **게임 데이터 I/O**. 경로, 포맷 파싱, 리소스 로드, 설정 저장, 직렬화. 파일·경로·시스템 UI(메시지박스 등)는 **PAL 인터페이스**를 통해 사용한다.
- **의존**: PAL 레이어(인터페이스: IFileSystem, IPathProvider, ILogger, IMessageBox 등). Common 사용 가능. (OS API·SDL 직접 사용 금지.)
- **모듈 및 클래스**:

| 모듈 | 설명 | 클래스 예 |
|------|------|-----------|
| **PathManager** | 리소스/설정/로그/세이브 경로 | `PathManager` |
| **ResourceIds** | 리소스 ID 열거 및 이름 매핑 | `ResourceId`, `ResourceCatalog` |
| **Formats** | BK, AF, REC, 언어, 스프라이트 등 파일 포맷 | `BkReader`, `AfLoader`, `RecFile`, `LanguageFile`, `Palette`, `VgaImage` 등 |
| **ResourceLoaders** | 게임 리소스 로드(폰트, 사운드, 언어, 토너먼트) | `BkLoader`, `SoundsLoader`, `FontLoader`, `TrnManager` |
| **Content** | **중립 런타임 모델** + **콘텐츠 백엔드** (확장용). 캐릭터·애니메이션·스테이지를 포맷에 무관한 구조로 로드. | `IContentBackend`, `NeutralCharacter`, `NeutralAnimation`, `NeutralStage`, `LegacyContentBackend`, `BlenderContentBackend` 등 (§3.9 참고) |
| **Settings** | 설정 저장/로드(비디오, 오디오, 게임플레이) | `Settings`, `SettingsStore` |
| **Serialization** | 네트워크/녹화용 직렬화 | `SerialBuffer`, `SerialReader`, `SerialWriter` |
| **System** | 로그, 메시지박스, 크래시 핸들러, 시간 포맷 (PAL의 ILogger/IMessageBox 사용) | `Logger`, `MessageBox`, `CrashHandler`, `TimeFormat` |

---

### 3.5 HAL (Hardware Abstraction Layer)

- **역할**: **하드웨어·디바이스 추상화**. GPU(렌더링), 오디오(사운드 디바이스), 입력(키보드·조이스틱·터치·게임패드)에 대한 **인터페이스**를 정의하고, **플랫폼·API별 구현**만 이 레이어에 둔다. Presentation은 이 인터페이스만 참조한다. 그래픽 컨텍스트 생성 등은 필요 시 PAL의 `IWindow`를 통해 창 핸들을 받아 사용한다.
- **의존**: PAL 레이어(인터페이스: IWindow, IPathProvider 등 필요 시). Common. 구현체는 OpenGL/Vulkan/DirectX, SDL_audio, FMOD 등 **HAL 내부에서만** 사용.
- **타겟 구현**: 플랫폼·API별 (예: OpenGL ES 공통, Vulkan, DirectX, Switch/PS 전용 API). `hal/opengl`, `hal/vulkan`, `hal/null` 등.
- **모듈 및 클래스** (인터페이스 + 구현):

| 모듈 | 설명 | 인터페이스 / 클래스 예 |
|------|------|------------------------|
| **Graphics** | 렌더링 컨텍스트, 버퍼, 셰이더, 그리기 | `IRenderDevice`, `IGraphicsContext`, `IRenderer` (Presentation이 사용) |
| **Audio** | 오디오 디바이스, 재생, 볼륨 | `IAudioDevice`, `IAudioBackend` (Presentation이 사용) |
| **Input** | 키보드·조이스틱·터치·게임패드 | `IInputProvider`, `IInputDevice` |
| **HALFactory** | 현재 빌드에 맞는 HAL 구현체 생성 | `HALFactory`, `CreateRenderDevice(window)` 등 |

- **규칙**: HAL 위 레이어는 **GPU/오디오/입력 관련 플랫폼 API, OpenGL/Vulkan/SDL 직접 호출**을 하지 않는다. HAL 구현체 소스(`hal/opengl/*.cpp` 등) 안에서만 사용한다.

---

### 3.6 PAL (Platform Abstraction Layer)

- **역할**: **OS·플랫폼 추상화**. 파일 시스템, 경로, 창(OS 창), 스레드, 시간, 로그, 메시지박스, 크래시 리포팅 등 **플랫폼(OS) 차이**를 흡수한다. Platform·HAL은 이 인터페이스만 참조한다.
- **의존**: Common. 구현체는 각 OS API(Windows API, POSIX, Android NDK, iOS, Switch SDK, PlayStation SDK) 또는 SDL 등 **PAL 내부에서만** 사용.
- **타겟 플랫폼 구현**: Windows, Linux, Android, iOS, Nintendo Switch, PlayStation, Xbox. `pal/win32`, `pal/linux`, `pal/android`, `pal/ios`, `pal/switch`, `pal/playstation`, `pal/xbox`, `pal/null`(테스트용) 등.
- **모듈 및 클래스** (인터페이스 + 플랫폼별 구현):

| 모듈 | 설명 | 인터페이스 / 클래스 예 |
|------|------|------------------------|
| **Window** | OS 창 생성, 크기, 풀스크린, vsync 옵션 | `IWindow`, `WindowConfig` |
| **FileSystem** | 파일 읽기/쓰기, 디렉터리 열거 | `IFileSystem` |
| **Path** | 앱 데이터·리소스·설정·로그 경로 | `IPathProvider` |
| **Time** | 틱, 프레임 간격, 시간 문자열 | `ITimeProvider`, `ITimer` |
| **Thread** | 스레드·뮤텍스 (필요 시) | `IThread`, `IMutex` |
| **System** | 로그, 메시지박스, 크래시 리포팅 | `ILogger`, `IMessageBox`, `ICrashHandler` |
| **PALFactory** | 현재 플랫폼에 맞는 PAL 구현체 생성 | `PALFactory`, `CreateWindow()`, `CreateFileSystem()` 등 |

- **규칙**: PAL 위 레이어는 **플랫폼 이름, `#ifdef`, OS 전용 헤더**를 사용하지 않는다. PAL 구현체 소스(`pal/win32/*.cpp` 등) 안에서만 사용한다.

---

### 3.7 Common (공통)

- **역할**: **모든 레이어에서 공통으로 쓰는** 유틸리티. 수학, 문자열, 컨테이너, 메모리, 타입 정의 등. 다른 게임 레이어에 의존하지 않음.
- **의존**: 표준 라이브러리(std) 및 외부 유틸만. Presentation/Application/Domain/Platform/HAL/PAL은 Common에 의존 가능.
- **모듈 및 클래스**:

| 모듈 | 설명 | 클래스 예 |
|------|------|-----------|
| **Math** | 벡터, 사칙, 보간, 충돌용 수학 | `Vec2`, `Vec2i`, `Rect`, `MathUtils` |
| **String** | 문자열 처리, 인코딩(CP437 등) | `StringUtil`, `Cp437` |
| **Container** | 배열, 맵, 링버퍼 등 (std 래퍼 또는 확장) | `ObjectPool`, `RingBuffer` (필요 시) |
| **Memory** | 할당자, 스마트 포인터 래퍼 | `Allocator`, `ScopedGuard` (필요 시) |
| **Types** | 공통 타입 별칭, 열거, 상수 | `Optional`, `Result` (필요 시), `Constants` |
| **Debug** | assert 확장, 디버그 헬퍼 | `Assert`, `DebugTimer` (필요 시) |

---

### 3.8 콘텐츠 확장(로봇·배경·BGM) 시 수정 레이어

새 로봇(HAR) 추가/제거, 전투 배경(아레나)·스토리 추가/제거, 배경음악(BGM) 추가/제거 시 **현재 구조에서 어떤 레이어를 수정해야 하는지** 정리한다.  
목표: 추가/제거가 쉬우려면 **가능한 한 Platform(데이터·매니페스트)만 수정**하고, Domain/Application/Presentation은 **목록을 데이터에서 읽어** 사용하도록 한다.

#### 3.8.1 새 로봇(HAR) 추가/제거 시

| 레이어 | 수정 여부 | 수정 내용 |
|--------|-----------|-----------|
| **Platform** | **필수** | **ResourceIds** — 새 HAR용 리소스 ID 추가/삭제(AF 파일, 필요 시 BK/이미지). **ResourceCatalog**·매니페스트에 항목 추가/삭제. 리소스 파일(AF 등) 배치. |
| **Domain** | **enum/상수 사용 시 필수** | HAR 종류가 enum·상수로 정의되어 있으면 **HAR 타입 enum**, **이름/표시용 상수·배열** 추가/삭제. 엔티티는 `Har` 하나로 두고 ID·데이터만 구분하면 Domain 코드 변경 최소화. |
| **Application** | 목록 하드코딩 시 필요 | 캐릭터 선택·메크랩 등에서 HAR 목록을 **enum/상수에 의존**하면 해당 메뉴/씬 로직 수정. 목록을 **Platform/Domain에서 데이터로 조회**하면 수정 불필요. |
| **Presentation** | 목록 하드코딩 시 필요 | 캐릭터 선택 GUI가 **고정 목록**이면 위젯·목록 소스 수정. **동적 목록**(리소스 카탈로그/매니페스트 기반)이면 수정 불필요. |
| **HAL / PAL / Common** | 불필요 | 수정하지 않음. |

#### 3.8.2 새 전투 배경(아레나)·스토리 추가/제거 시

| 레이어 | 수정 여부 | 수정 내용 |
|--------|-----------|-----------|
| **Platform** | **필수** | **ResourceIds** — 새 BK(배경)·씬·스토리용 ID 추가/삭제. **ResourceLoaders**·경로 규칙에서 해당 리소스 로드 가능하도록. BK·스크립트 등 **리소스 파일** 배치. |
| **Domain** | **enum/상수 사용 시 필수** | **ArenaId**·**SceneId**·스토리 구간 등이 enum/상수면 추가/삭제. **ArenaConstraints**·배경 참조가 데이터 주도면 Domain 코드 변경 최소화. |
| **Application** | 씬/아레나 목록 하드코딩 시 필요 | **SceneFactory**, 아레나 선택 메뉴 등에서 **씬/아레나 목록이 코드에 고정**되어 있으면 해당 목록·분기 수정. 목록을 **Platform/Domain의 카탈로그·매니페스트에서 조회**하면 수정 불필요. |
| **Presentation** | 목록 하드코딩 시 필요 | 아레나 선택·스토리 분기 UI가 **고정 목록**이면 수정. **동적 목록**이면 수정 불필요. |
| **HAL / PAL / Common** | 불필요 | 수정하지 않음. |

#### 3.8.3 배경음악(BGM) 추가/제거 시

| 레이어 | 수정 여부 | 수정 내용 |
|--------|-----------|-----------|
| **Platform** | **필수** | **ResourceIds** — 새 BGM 리소스 ID 추가/삭제(PSM/OGG 등). **PathManager**·리소스 경로에 음원 파일 배치. 재생은 ID 기반이면 포맷/로더 코드는 그대로 두고 **데이터(매니페스트)·파일만** 추가/삭제. |
| **Domain** | 선택 | 아레나·씬과 BGM 매핑이 Domain 규칙이면 해당 매핑 데이터/상수. 없으면 수정 불필요. |
| **Application** | 매핑 하드코딩 시 필요 | “아레나 N → BGM ID” 등 **매핑이 코드에 고정**되어 있으면 추가/삭제 시 수정. **아레나/씬 메타데이터(Platform)에 BGM ID 포함**하면 Application은 그대로 재생만 호출하면 되므로 수정 불필요. |
| **Presentation** | 불필요 | 재생은 ID 기반이므로 수정하지 않음. |
| **HAL / PAL / Common** | 불필요 | 수정하지 않음. |

#### 3.8.4 확장 용이 설계 권고

- **Platform에 콘텐츠 카탈로그/매니페스트 도입**: 로봇·아레나·BGM 목록을 **코드 enum이 아닌 데이터**(JSON/INI/바이너리 매니페스트)로 관리. 추가/제거 시 **매니페스트와 리소스 파일만** 변경하고, **ResourceIds**는 “매니페스트에서 ID 조회” 방식으로 확장.
- **Domain**: HAR/아레나/씬을 **타입 enum 확장**이 아니라 **ID·키 기반**으로 식별. 이름·제약 등은 Platform이 로드한 메타데이터에서 주입.
- **Application / Presentation**: 로봇 선택·아레나 선택·BGM 재생은 **항상 Platform(또는 Domain)이 제공하는 목록/메타데이터**를 사용하고, **고정 enum/switch 분기**를 피하면, 새 로봇·배경·BGM 추가/제거 시 **수정 레이어는 사실상 Platform(데이터·매니페스트)** 으로 한정할 수 있다.

---

### 3.9 애니메이션·모델 파이프라인 확장 (Blender / Unity / Unreal 등)

우선 **C 디렉터리를 그대로 리팩토링**할 때는 기존 AF/BK/스프라이트 포맷을 유지한다. 이후 **Blender, Unity, Unreal** 등으로 제작한 모델·애니메이션을 쓸 수 있도록 확장하려면, **포맷에 의존하지 않는 중립 런타임 모델**과 **콘텐츠 백엔드** 구조가 필요하다.

#### 3.9.1 설계 원칙

- **Domain·Application은 “캐릭터·애니메이션·스테이지”의 포맷을 모른다.**  
  리소스 ID 또는 핸들로 **중립 구조체**(캐릭터 데이터, 애니메이션 클립, 스테이지 데이터)만 참조한다.
- **Platform(Content)** 에서만 **파일 포맷(AF, BK, glTF, FBX, Unity/Unreal export 등)** 을 알고, 이를 **중립 런타임 모델**로 변환해 제공한다.
- **Presentation** 은 **중립 모델**을 받아 그리기만 한다.  
  현재는 2D 스프라이트/서페이스, 확장 시 3D 메시·스켈레톤 애니메이션 등 **다른 렌더 백엔드**를 둘 수 있다.

#### 3.9.2 중립 런타임 모델 (Platform · Content)

게임 로직과 렌더러가 공통으로 의존하는 **포맷 중립** 구조.  
구체적인 필드는 프로젝트 진행 시 정의하되, 개념은 아래와 같이 둔다.

| 개념 | 역할 | 예시 (현재 확장 시) |
|------|------|---------------------|
| **NeutralCharacter** | 한 캐릭터(로봇)의 **표시·애니메이션 가능한** 데이터. 포맷 무관. | 스프라이트 시트 + 클립 목록 (레거시) / 스켈레톤 + 메시 + 클립 (Blender·Unity·Unreal) |
| **NeutralAnimation** | 재생 가능한 **애니메이션 클립** 하나. 프레임/키프레임, 이벤트(타격 프레임 등). | 스프라이트 인덱스 시퀀스 (레거시) / 스켈레톤 클립 또는 메시 애니메이션 |
| **NeutralStage** | 전투 배경·스테이지 한 개. | 배경 이미지·레이어 + 팔레트 (레거시) / 3D 씬 또는 배경 메시 |

- **Domain** 은 “캐릭터 ID, 현재 애니메이션 클립 ID, 프레임, 방향” 등 **논리적 상태**만 갖고, NeutralCharacter/NeutralAnimation 의 **내부 포맷(스프라이트 vs 메시)** 에 의존하지 않는다.
- **Presentation** 은 NeutralCharacter + 현재 클립 + 프레임을 받아, **2D 렌더러**는 스프라이트로, **3D 렌더러**는 메시·스켈레톤으로 그린다.

#### 3.9.3 콘텐츠 백엔드 (Platform · Content)

**IContentBackend** (또는 유사 인터페이스): 리소스 ID(또는 경로)로 **NeutralCharacter**, **NeutralAnimation**, **NeutralStage** 를 로드한다.

| 백엔드 | 용도 | 입력 포맷 | 출력 |
|--------|------|-----------|------|
| **LegacyContentBackend** | C 리팩토링 단계. 기존 데이터 그대로 사용. | AF, BK, 스프라이트, PCX 등 | NeutralCharacter / NeutralAnimation / NeutralStage (2D 스프라이트 기반) |
| **BlenderContentBackend** | 확장. Blender 제작 파이프라인. | glTF, FBX, 또는 프로젝트 규약 export | 동일 중립 모델 (메시·스켈레톤·클립) |
| **UnityExportBackend** | 확장. Unity에서 익스포트한 데이터. | 프로젝트 규약(JSON+에셋, FBX 등) | 동일 중립 모델 |
| **UnrealExportBackend** | 확장. Unreal에서 익스포트한 데이터. | 프로젝트 규약(FBX, 애니메이션 시퀀스 등) | 동일 중립 모델 |

- **리팩토링 단계**: LegacyContentBackend 만 구현. 기존 AF/BK/Formats/ResourceLoaders 를 활용해 Neutral* 를 채우면 된다.
- **확장 단계**: Blender/Unity/Unreal용 백엔드를 추가하고, 매니페스트 또는 리소스 타입에 따라 사용할 백엔드를 선택한다. Domain·Application·Presentation 의 **공개 인터페이스는 그대로** 두고, Platform(Content) 과 Presentation(렌더 백엔드) 만 확장한다.

#### 3.9.4 Presentation(렌더) 측 확장

- **현재**: 중립 모델 중 **2D 스프라이트/서페이스**만 사용하는 **SpriteRenderer** (기존 Video·Surface·VgaState 에 해당).
- **확장**: 중립 모델이 **메시·스켈레톤·클립**을 담고 있으면 **ModelRenderer** (또는 SkeletalRenderer) 가 HAL(IRenderDevice) 위에 구현되어, 같은 **캐릭터 상태(클립 ID, 프레임)** 를 3D로 그린다.
- Domain·Application 은 “현재 애니메이션, 프레임”만 전달하고, **어떤 렌더러가 그리는지**는 Presentation 내부에서 중립 모델 타입 또는 설정에 따라 분기한다.

#### 3.9.5 레이어별 수정 범위 요약

| 단계 | Platform | Domain | Application | Presentation |
|------|----------|--------|-------------|--------------|
| **C 리팩토링** | Formats·ResourceLoaders 유지, **Content** 모듈 추가. LegacyContentBackend 로 AF/BK → Neutral* 변환. | 캐릭터·애니메이션을 **ID + Neutral* 참조**로 사용하도록 정리. (기존 로직 유지 가능) | 리소스 로드 시 ContentBackend 사용. | 기존 2D 렌더링을 Neutral* 기반으로 동작하도록 정리. |
| **Blender/Unity/Unreal 확장** | BlenderContentBackend 등 **새 백엔드** 추가. 중립 모델 필드 확장(메시·스켈레톤). | 변경 없음 (이미 중립 참조). | 변경 없음. | **ModelRenderer** 등 새 렌더 백엔드 추가. |

이 구조로 가면 **먼저 C를 그대로 리팩토링**한 뒤, **나중에 Blender/Unity/Unreal 파이프라인**은 Platform(Content 백엔드) 과 Presentation(렌더 백엔드) 확장만으로 수용할 수 있다.

---

## 4. 코딩 스타일

### 4.1 파일명과 클래스명

- **클래스 이름과 해당 파일 이름은 동일해야 한다.**  
  - 예: `GameState` 클래스 → `GameState.hpp` / `GameState.cpp`.  
  - 추상 클래스(인터페이스)는 접두사 `I` 사용 시 `IRenderer.hpp` 등으로 파일명도 동일하게 둔다.

### 4.2 명명 규칙

- **클래스·구조체·열거형·타입 별칭**  
  - **대문자로 시작하는 카멜 표기법(PascalCase)**.  
  - 예: `GameState`, `VgaPalette`, `IRenderer`, `ResourceId`.

- **클래스 내부 멤버 변수**  
  - **`m_` 접두사** + **소문자와 언더스코어(snake_case)**.  
  - 예: `m_next_scene_id`, `m_match_settings`, `m_player_index`.

- **클래스 내부 멤버 함수**  
  - **소문자와 언더스코어(snake_case)**.  
  - 예: `get_next_scene()`, `load_har()`, `set_player_index()`.

- **멤버 함수의 매개변수(인자)**  
  - **`a_` 접두사** + **소문자와 언더스코어(snake_case)**.  
  - 예: `void set_id(int a_scene_id);`, `void load(const std::string& a_path);`.

- **지역 변수**  
  - **소문자와 언더스코어(snake_case)**. 직관적으로 이해하기 쉬운 이름 사용.  
  - 예: `scene_id`, `player_index`.

- **상수, 열거자, 매크로**  
  - **UPPER_SNAKE_CASE** (매크로), **kPascalCase** 또는 UPPER_SNAKE_CASE (constexpr·열거자).  
  - 예: `MAX_PLAYERS`, `kNativeWidth`.

- **네임스페이스**  
  - **snake_case**. 예: `openomf::video`, `openomf::domain`.

- **작명**  
  - **직관적이고 이해하기 쉽게** 짓는다. 역할·의도가 드러나도록 한다.

### 4.3 포맷팅

- **들여쓰기**: 스페이스 **4칸**.
- **한 줄 길이**: **120자** 이내 권장 (넘을 경우 줄바꿈, 정렬 스타일 팀 합의).
- **중괄호**: K&R 스타일. 함수/클래스/제어문 `{` 다음 줄 또는 같은 줄 팀 합의 후 통일.
- **줄 끝**: LF (Unix style).

### 4.4 SOLID 원칙

다음 **SOLID** 원칙을 준수한다.

| 원칙 | 설명 | 적용 |
|------|------|------|
| **S (Single Responsibility)** | 단일 책임: 한 클래스는 하나의 변경 이유만 가져야 한다. | 클래스는 한 가지 역할만 담당. 여러 책임이 보이면 분리. |
| **O (Open/Closed)** | 개방-폐쇄: 확장에는 열려 있고 수정에는 닫혀 있어야 한다. | 새 로봇·씬·백엔드 추가 시 **기존 코드 수정 없이** 추상 클래스 구현체 추가로 확장. |
| **L (Liskov Substitution)** | 리스코프 치환: 파생 타입은 기반 타입을 대체해도 동작이 깨지지 않아야 한다. | 상속/구현 시 계약(선행·후행 조건, 불변식)을 유지. 기반 클래스 포인터로 교체해도 논리가 유지되도록. |
| **I (Interface Segregation)** | 인터페이스 분리: 클라이언트가 쓰지 않는 메서드에 의존하지 않도록 인터페이스를 작게 나눈다. | 굵은 인터페이스 대신 **역할별로 쪼갠 인터페이스**(예: IRenderable, ILoadable 분리). |
| **D (Dependency Inversion)** | 의존성 역전: 구체가 아닌 추상에 의존해야 한다. | 상위 모듈은 `IRenderer`, `IContentBackend` 등 **추상(인터페이스)** 에만 의존. 구현체는 주입·팩토리로 연결. |

- **추상 클래스**를 적절히 사용하여 확장 가능하게 설계한다.  
  예: 확장이 예상되는 **로봇(캐릭터)** 객체, **배경/씬** 객체, **콘텐츠 백엔드**, **렌더러** 등은 추상 클래스(인터페이스)로 정의하고, 구체 타입은 그 구현체로 관리한다.

### 4.5 C++ 관례 및 표준 라이브러리

- **const**: 변경 불가 인자·멤버·반환은 `const` 사용. getter는 `const` 메서드.
- **스마트 포인터**: 소유권이 있을 경우 `std::unique_ptr` / `std::shared_ptr` 사용.裸 포인터는 “참조만” 할 때.
- **RAII**: 리소스(파일, 핸들, GL 객체 등)는 생성자에서 획득, 소멸자에서 해제.
- **override / final**: 가상 함수 재정의 시 `override`, 더 이상 파생 금지 시 `final` 명시.
- **헤더**: 필요한 최소 include. 전방 선언으로 해결 가능하면 include 대체.
- **네임스페이스**: 프로젝트 루트 네임스페이스 예: `openomf`. 레이어/모듈별 하위 네임스페이스 사용 권장.
- **표준 라이브러리·STL**: **std 라이브러리를 최대한 활용**한다. `std::map`, `std::vector`, `std::string`, `std::optional`, `std::variant`, `std::unique_ptr` 등 STL을 **적극 활용**한다. 자체 구현 전에 std로 해결 가능한지 검토한다.

### 4.6 객체지향 C++ 및 이식 시 원칙

- **기존 C 코드를 C++로 옮길 때 단순 함수형으로 옮기지 않는다.**  
  **응집력이 높은 클래스**로 묶어서 설계한다. 관련 데이터와 동작을 한 클래스에 두고, 책임이 나뉘면 클래스로 분리한다.
- **`extern "C"`를 이용해 이름만 C++이고 실제로는 C인 래퍼 형식으로 구현해서는 안 된다.**  
  **진짜 객체지향 C++**로 구현한다. 상태와 동작을 클래스로 캡슐화하고, 상속·조합·추상 인터페이스를 사용한다.
- **확장 예상 대상(로봇, 배경/씬, 백엔드 등)은 추상 클래스로 추상화하여 관리한다.**  
  구체 타입은 해당 추상의 구현체로 추가하고, 상위 코드는 추상에만 의존하도록 한다(§4.4 SOLID).

### 4.7 금지·자제

- **매크로**: 상수/인라인 대체 가능하면 매크로 자제. 매크로 사용 시 UPPER_SNAKE_CASE.
- **전역 가변 상태**: 가능한 한 클래스/DI로 캡슐화.
- **using namespace std**: 헤더에서는 사용 금지. 구현 파일에서도 좁은 스코프 권장.

---

## 5. 파일 스타일

### 5.1 확장자 및 헤더 규칙

- **헤더**: 반드시 **`.hpp`** 확장자 사용. `.h` + `.cpp` 조합이 아니라 **헤더는 .hpp 형태**로 통일한다.  
  - C 호환 헤더가 꼭 필요한 경우만 예외적으로 `.h` 및 `extern "C"` 검토.
- **구현**: `.cpp`.
- **.hpp 파일**:  
  - **가독성(리더빌리티)이 좋아야 한다.**  
  - **최대 300라인을 넘지 않는다.** 넘어가면 클래스 분리·모듈 분할·전방 선언으로 줄인다.

### 5.2 파일 명명 및 클래스명 일치

- **클래스 이름과 파일 이름은 동일해야 한다** (§4.1).  
  - **한 파일에 한 클래스**를 두는 것을 기본으로 한다.  
  - 파일명 = 클래스명 (대문자로 시작하는 카멜 표기법).  
  - 예: `GameState` 클래스 → `GameState.hpp` / `GameState.cpp`, `IRenderer` → `IRenderer.hpp`.
- **인터페이스(추상 클래스)**  
  - 접두사 `I` (예: `IRenderer.hpp`, `IAudioBackend.hpp`).
- **작은 타입/유틸**  
  - 여러 개를 한 헤더에 모을 수 있음. 이때도 **300라인 이내**를 유지하고, 파일명은 모듈/역할에 맞게 (예: `Types.hpp`, `Enums.hpp`).

### 5.3 디렉터리 구조 (레이어·모듈 반영)

```
CPP/
├── requirement.md
├── src/
│   ├── presentation/    # Presentation (표현)
│   │   ├── video/
│   │   ├── vga/
│   │   ├── audio/
│   │   ├── input/
│   │   ├── console/
│   │   └── gui/
│   ├── application/      # Application (응용)
│   │   ├── engine/
│   │   ├── scene/
│   │   ├── gameloop/
│   │   ├── menus/
│   │   ├── network/
│   │   └── recording/
│   ├── domain/           # Domain (도메인)
│   │   ├── state/
│   │   ├── player/
│   │   ├── object/
│   │   ├── combat/
│   │   ├── arena/
│   │   └── match/
│   ├── platform/         # Platform (데이터)
│   │   ├── path/
│   │   ├── resource/
│   │   ├── formats/
│   │   ├── content/      # 중립 모델 + 콘텐츠 백엔드 (§3.9)
│   │   │   ├── neutral/  # NeutralCharacter, NeutralAnimation, NeutralStage
│   │   │   └── backends/
│   │   │       ├── legacy/   # AF/BK/스프라이트 → 중립
│   │   │       ├── blender/  # (확장) glTF 등
│   │   │       ├── unity_export/
│   │   │       └── unreal_export/
│   │   ├── settings/
│   │   ├── serialization/
│   │   └── system/
│   ├── hal/              # HAL (Hardware Abstraction Layer)
│   │   ├── interfaces/   # IRenderDevice, IAudioDevice, IInputProvider
│   │   ├── opengl/
│   │   ├── vulkan/       # (선택)
│   │   └── null/         # 테스트용 널 구현
│   ├── pal/              # PAL (Platform Abstraction Layer)
│   │   ├── interfaces/   # IWindow, IFileSystem, IPathProvider, ILogger 등
│   │   ├── win32/
│   │   ├── linux/
│   │   ├── android/
│   │   ├── ios/
│   │   ├── switch/
│   │   ├── playstation/
│   │   ├── xbox/
│   │   └── null/         # (선택) 테스트용 널 구현
│   └── common/           # Common (공통)
│       ├── math/
│       ├── string/
│       ├── container/
│       ├── memory/
│       ├── types/
│       └── debug/
├── include/              # (선택) 공개 API만 놓을 경우
└── resources/            # 셰이더, 데이터 등 (C와 공유 또는 링크)
```

- **include 경로**: `#include` 시 레이어/모듈 기준 상대 경로 또는 프로젝트 루트 기준 경로 중 하나로 통일.  
  - 예: `#include "application/engine/Engine.hpp"` 또는 `#include "engine/Engine.hpp"` (include path에 `src` 또는 `src/application` 포함).

### 5.4 헤더 가드

- **Include guard** 사용: `#ifndef OPENOMF_모듈_파일명_HPP` / `#define` / `#endif`  
  - 또는 **#pragma once** (사용 시 프로젝트 전역 통일).
- 매크로 이름: **UPPER_SNAKE_CASE**, 프로젝트 접두사 포함 (예: `OPENOMF_ENGINE_HPP`).

### 5.5 문서화

- **공개 API**: Doxygen 스타일 주석 권장 (`///`, `@param`, `@return`, `@brief`).
- **복잡한 비즈니스 로직**: 한글/영어 주석으로 의도 명시.

---

## 6. C → CPP 매핑 참고

| C (대상) | CPP 레이어 | CPP 모듈/클래스 참고 |
|----------|------------|----------------------|
| video.c/h, surface, renderers | Presentation | Video, Surface, IRenderer, Gl3Renderer |
| vga_state, vga_palette, vga_remap | Presentation | VgaState, VgaPalette, VgaRemap |
| audio.c/h, backends, sources | Presentation | AudioSystem, IAudioBackend, *Source |
| controller*, keyboard, joystick | Presentation | Input (Controller, Keyboard, …) |
| net_controller | Application | NetworkSession |
| console | Presentation | Console |
| gui/* | Presentation | Gui (Widget, Button, Menu, …) |
| engine.c/h | Application | Engine |
| game_state, scene (protos) | Application + Domain | SceneManager, IScene, GameState (Domain) |
| object, har, projectile, scrap, hazard | Domain | Object, Har, Projectile, Scrap, Hazard |
| game_player, match_settings | Domain | GamePlayer, MatchSettings |
| formats/* | Platform | Formats (BkReader, AfLoader, …) |
| pathmanager, ids, *loader, *manager | Platform | PathManager, ResourceIds, ResourceLoaders |
| settings | Platform | Settings |
| serial | Platform | Serialization |
| utils (log, msgbox, time, …) | Platform | System (Logger, MessageBox, TimeFormat) |
| utils (vector, list, hashmap, str, random, …) | Common | Math, String, Container, Types |
| GPU·오디오·입력 (OpenGL, SDL_audio 등) | HAL | IRenderDevice, IAudioDevice, IInputProvider, hal/opengl 등 |
| 파일·경로·창·스레드·로그, OS API, #ifdef | PAL | IWindow, IFileSystem, IPathProvider, ILogger, pal/win32, pal/android 등 |

---

## 7. 문서 갱신

- 레이어/모듈/클래스 추가·변경 시 이 문서를 갱신한다.
- 코딩/파일 스타일 예외는 팀 합의 후 이 문서에 명시한다.

---

*작성 후 추가 편집은 프로젝트 담당자가 수행합니다.*
