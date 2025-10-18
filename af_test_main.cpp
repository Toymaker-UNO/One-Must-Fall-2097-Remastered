#include <iostream>
#include <string>
#include <SDL.h>

extern "C" {
#include "src/formats/af.h"
#include "src/formats/bk.h"
#include "src/formats/sprite.h"
#include "src/formats/rgba_image.h"
#include "src/formats/error.h"
}

// AF 파일 로더 및 렌더러
class AFTester {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    SDL_Texture* renderTarget;
    sd_af_file af_file;
    sd_bk_file bk_file;
    bool initialized;
    
public:
    AFTester() : window(nullptr), renderer(nullptr), texture(nullptr), 
                 renderTarget(nullptr), initialized(false) {
        sd_af_create(&af_file);
        sd_bk_create(&bk_file);
    }
    
    ~AFTester() {
        cleanup();
        sd_af_free(&af_file);
        sd_bk_free(&bk_file);
    }
    
    bool loadAF(const std::string& afPath) {
        int ret = sd_af_load(&af_file, afPath.c_str());
        if (ret != SD_SUCCESS) {
            std::cerr << "AF file load failed: " << sd_get_error((SD_ERRORCODE)ret) << std::endl;
            return false;
        }
        std::cout << "AF file loaded successfully: " << afPath << std::endl;
        return true;
    }
    
    bool loadPalette(const std::string& bkPath) {
        int ret = sd_bk_load(&bk_file, bkPath.c_str());
        if (ret != SD_SUCCESS) {
            std::cerr << "BK file load failed: " << sd_get_error((SD_ERRORCODE)ret) << std::endl;
            return false;
        }
        std::cout << "Palette file loaded successfully: " << bkPath << std::endl;
        return true;
    }
    
    bool initSDL(int scale = 3) {
        window = SDL_CreateWindow("AF Test - One Must Fall 2097",
                                 SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                 320 * scale, 200 * scale,
                                 SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
        
        if (!window) {
            std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
            return false;
        }
        
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer) {
            std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
            return false;
        }
        
        // 320x200 렌더 타겟 생성
        renderTarget = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                        SDL_TEXTUREACCESS_TARGET, 320, 200);
        if (!renderTarget) {
            std::cerr << "Render target creation failed: " << SDL_GetError() << std::endl;
            return false;
        }
        
        initialized = true;
        return true;
    }
    
    void playFirstAnimation() {
        if (!initialized) {
            std::cerr << "SDL is not initialized." << std::endl;
            return;
        }
        
        // 첫 번째 유효한 애니메이션 찾기
        int animIndex = -1;
        for (int i = 0; i < MAX_AF_MOVES; i++) {
            if (af_file.moves[i] != nullptr) {
                animIndex = i;
                break;
            }
        }
        
        if (animIndex == -1) {
            std::cerr << "No available animations found." << std::endl;
            return;
        }
        
        std::cout << "Starting animation " << animIndex << std::endl;
        
        sd_move* move = af_file.moves[animIndex];
        sd_animation* anim = move->animation;
        
        if (!anim || anim->sprite_count == 0) {
            std::cerr << "Animation has no sprites." << std::endl;
            return;
        }
        
        std::cout << "Sprite count: " << (int)anim->sprite_count << std::endl;
        std::cout << "Start position: (" << anim->start_x << ", " << anim->start_y << ")" << std::endl;
        
        // 애니메이션 루프
        bool running = true;
        int currentSprite = 0;
        SDL_Event event;
        
        while (running) {
            //std::cout << "running loop begin" << std::endl;
            // 이벤트 처리
            while (SDL_PollEvent(&event)) {
                //std::cout << "SDL_PollEvent loop begin" << std::endl;
                switch (event.type) {
                    case SDL_QUIT:
                        running = false;
                        break;
                    case SDL_KEYDOWN:
                        switch (event.key.keysym.sym) {
                            case SDLK_ESCAPE:
                                running = false;
                                break;
                            case SDLK_RIGHT:
                                currentSprite = (currentSprite + 1) % anim->sprite_count;
                                std::cout << "Next sprite: " << currentSprite << std::endl;
                                break;
                            case SDLK_LEFT:
                                currentSprite = (currentSprite - 1 + anim->sprite_count) % anim->sprite_count;
                                std::cout << "Previous sprite: " << currentSprite << std::endl;
                                break;
                        }
                        break;
                }
                //std::cout << "SDL_PollEvent loop end" << std::endl;
            }
            //std::cout << "SDL_PollEvent loop break" << std::endl;
            
            //std::cout << "current sprite rendering" << std::endl;
            // 현재 스프라이트 렌더링
            renderSprite(anim, currentSprite);
            
            //std::cout << "update screen" << std::endl;
            // 화면 업데이트
            SDL_RenderPresent(renderer);
            SDL_Delay(100); // 10 FPS
            //std::cout << "running loop end" << std::endl;
        }
    }
    
private:
    void renderSprite(sd_animation* anim, int spriteIndex) {
        if (spriteIndex >= anim->sprite_count || !anim->sprites[spriteIndex]) {
            return;
        }
        
        sd_sprite* sprite = anim->sprites[spriteIndex];
        
        // 스프라이트를 RGBA 이미지로 디코딩
        sd_rgba_image img;
        int ret = sd_sprite_rgba_decode(&img, sprite, bk_file.palettes[0]);
        if (ret != SD_SUCCESS) {
            std::cerr << "Sprite decoding failed: " << sd_get_error((SD_ERRORCODE)ret) << std::endl;
            return;
        }
        
        // SDL Surface 생성
        SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(
            (void*)img.data, img.w, img.h, 32, img.w * 4,
            0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
        
        if (!surface) {
            std::cerr << "Surface creation failed: " << SDL_GetError() << std::endl;
            sd_rgba_image_free(&img);
            return;
        }
        
        // 기존 텍스처 해제
        if (texture) {
            SDL_DestroyTexture(texture);
        }
        
        // 새 텍스처 생성
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        sd_rgba_image_free(&img);
        
        if (!texture) {
            std::cerr << "Texture creation failed: " << SDL_GetError() << std::endl;
            return;
        }
        
        // 렌더링
        SDL_RenderClear(renderer);
        SDL_SetRenderTarget(renderer, renderTarget);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, nullptr);
        
        // Calculate sprite position (centered on screen)
        SDL_Rect destRect;
        destRect.x = anim->start_x + sprite->pos_x + 160 - sprite->width / 2;
        destRect.y = anim->start_y + sprite->pos_y + 100 - sprite->height / 2;
        destRect.w = sprite->width;
        destRect.h = sprite->height;
        
        SDL_RenderCopy(renderer, texture, nullptr, &destRect);
        
        // Visualize collision coordinates (red dots)
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        for (int i = 0; i < anim->coord_count; i++) {
            if (anim->coord_table[i].frame_id == spriteIndex) {
                int x = anim->start_x + anim->coord_table[i].x + 160;
                int y = anim->start_y + anim->coord_table[i].y + 100;
                SDL_RenderDrawPoint(renderer, x, y);
            }
        }
        
        // Scale and output to screen
        SDL_SetRenderTarget(renderer, nullptr);
        SDL_Rect screenRect = {0, 0, 320 * 3, 200 * 3};
        SDL_RenderCopy(renderer, renderTarget, nullptr, &screenRect);
    }
    
    void cleanup() {
        if (texture) {
            SDL_DestroyTexture(texture);
            texture = nullptr;
        }
        if (renderTarget) {
            SDL_DestroyTexture(renderTarget);
            renderTarget = nullptr;
        }
        if (renderer) {
            SDL_DestroyRenderer(renderer);
            renderer = nullptr;
        }
        if (window) {
            SDL_DestroyWindow(window);
            window = nullptr;
        }
    }
};

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " <AF_file_path> [BK_file_path]" << std::endl;
    std::cout << "Example: " << programName << " resources/game_resources/FIGHTR0.AF resources/game_resources/ARENA0.BK" << std::endl;
    std::cout << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  Left/Right arrows: Previous/Next sprite" << std::endl;
    std::cout << "  ESC: Exit" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    
    std::string afPath = argv[1];
    std::string bkPath = (argc >= 3) ? argv[2] : "resources/game_resources/ARENA0.BK";
    
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        exit(1);
    }
    
    // Create AF tester
    AFTester tester;
    
    // Load AF file
    if (!tester.loadAF(afPath)) {
        return 1;
    }
    
    // Load palette file
    if (!tester.loadPalette(bkPath)) {
        return 1;
    }
    
    // Initialize SDL
    if (!tester.initSDL()) {
        return 1;
    }
    
    // Play first animation
    tester.playFirstAnimation();
    
    SDL_Quit();
    return 0;
}

