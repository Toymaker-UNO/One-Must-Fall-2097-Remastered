// sdl_toggle_two_bmp.cpp
//
// Build example:
//   Windows (MinGW):
//     g++ sdl_toggle_two_bmp.cpp -o app -I"C:/SDL2/include" -L"C:/SDL2/lib" -lmingw32 -lSDL2main -lSDL2
//   Linux:
//     g++ sdl_toggle_two_bmp.cpp -o app `sdl2-config --cflags --libs`
//   macOS (Homebrew):
//     g++ sdl_toggle_two_bmp.cpp -o app -I/opt/homebrew/include -L/opt/homebrew/lib -lSDL2
//
// Run:
//   ./app first.bmp second.bmp

#include <SDL2/SDL.h>
#include <iostream>
#include <string>

// 창 크기 기본값
static const int WIN_W = 960;
static const int WIN_H = 540;

// 현재 윈도우 크기에 맞춰 텍스처를 화면 중앙에 "비율 유지"로 그릴 목적지 사각형 계산
SDL_Rect FitRectToWindow(SDL_Texture* tex, int winW, int winH) {
    int tw = 0, th = 0;
    SDL_QueryTexture(tex, nullptr, nullptr, &tw, &th);

    // 대상 크기 비율 유지 (letterbox)
    double scale = std::min(static_cast<double>(winW) / tw,
                            static_cast<double>(winH) / th);
    int dw = static_cast<int>(tw * scale);
    int dh = static_cast<int>(th * scale);

    SDL_Rect dst;
    dst.w = dw;
    dst.h = dh;
    dst.x = (winW - dw) / 2;
    dst.y = (winH - dh) / 2;
    return dst;
}

// 오류 찍고 정리 후 종료
[[noreturn]] void Fail(const std::string& msg) {
    std::cerr << "[ERROR] " << msg << "\nSDL: " << SDL_GetError() << std::endl;
    SDL_Quit();
    std::exit(1);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << (argc > 0 ? argv[0] : "app") << " <first.bmp> <second.bmp>\n";
        return 1;
    }
    std::string pathA = argv[1];
    std::string pathB = argv[2];

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        Fail("SDL_Init failed");
    }

    SDL_Window* window = SDL_CreateWindow(
        "SDL Toggle Two BMP (← → ↑ ↓ to switch, ESC to quit)",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIN_W, WIN_H,
        SDL_WINDOW_RESIZABLE);
    if (!window) Fail("SDL_CreateWindow failed");

    // 가속 렌더러 + VSync
    SDL_Renderer* renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) Fail("SDL_CreateRenderer failed");

    // BMP 파일 로드
    SDL_Surface* surfaceA = SDL_LoadBMP(pathA.c_str());
    if (!surfaceA) Fail("Failed to load first BMP file: " + pathA);
    
    SDL_Surface* surfaceB = SDL_LoadBMP(pathB.c_str());
    if (!surfaceB) Fail("Failed to load second BMP file: " + pathB);

    // Surface를 Texture로 변환
    SDL_Texture* tex[2] = { nullptr, nullptr };
    tex[0] = SDL_CreateTextureFromSurface(renderer, surfaceA);
    if (!tex[0]) Fail("Failed to create texture from first BMP");
    
    tex[1] = SDL_CreateTextureFromSurface(renderer, surfaceB);
    if (!tex[1]) Fail("Failed to create texture from second BMP");

    // Surface 메모리 해제
    SDL_FreeSurface(surfaceA);
    SDL_FreeSurface(surfaceB);

    // 창 타이틀에 파일명 간단 표기
    std::string title = "Showing: " + pathA + "  |  (switch with arrows, ESC quit)";
    SDL_SetWindowTitle(window, title.c_str());

    int current = 0; // 0 또는 1
    bool running = true;
    int winW = WIN_W, winH = WIN_H;

    // 키보드 자동 반복에 따른 과도 토글 방지: KEYDOWN만 처리 (repeat=0)
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_QUIT:
                running = false;
                break;
            case SDL_WINDOWEVENT:
                if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    winW = e.window.data1;
                    winH = e.window.data2;
                }
                break;
            case SDL_KEYDOWN:
                if (e.key.repeat) break; // 키 반복 무시
                switch (e.key.keysym.sym) {
                case SDLK_ESCAPE:
                    running = false;
                    break;
                case SDLK_LEFT:
                case SDLK_RIGHT:
                case SDLK_UP:
                case SDLK_DOWN:
                    current = 1 - current; // 0<->1 토글
                    SDL_SetWindowTitle(window,
                        (std::string("Showing: ") +
                         (current == 0 ? pathA : pathB) +
                         "  |  (switch with arrows, ESC quit)").c_str());
                    break;
                default:
                    break;
                }
                break;
            default:
                break;
            }
        }

        // 배경 지우기 (짙은 회색)
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);

        SDL_Rect dst = FitRectToWindow(tex[current], winW, winH);
        SDL_RenderCopy(renderer, tex[current], nullptr, &dst);

        SDL_RenderPresent(renderer);
    }

    // 정리
    if (tex[0]) SDL_DestroyTexture(tex[0]);
    if (tex[1]) SDL_DestroyTexture(tex[1]);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}