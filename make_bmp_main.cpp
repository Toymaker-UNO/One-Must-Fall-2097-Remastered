// make_bmp_main.cpp
// AF 파일에서 모든 로봇 애니메이션을 BMP 파일로 변환하는 프로그램
//
// Build example:
//   Windows (MinGW):
//     g++ make_bmp_main.cpp -o make_bmp -I"src" -I"lib/include" -L"lib/bin" -lmingw32 -lSDL2main -lSDL2 -lpng -lzlib
//   Linux:
//     g++ make_bmp_main.cpp -o make_bmp `sdl2-config --cflags --libs` -lpng -lzlib
//   macOS (Homebrew):
//     g++ make_bmp_main.cpp -o make_bmp -I/opt/homebrew/include -L/opt/homebrew/lib -lSDL2 -lpng -lzlib
//
// Run:
//   ./make_bmp <AF_file_path> [BK_file_path] [output_directory]

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <SDL2/SDL.h>

extern "C" {
#include "src/formats/af.h"
#include "src/formats/bk.h"
#include "src/formats/sprite.h"
#include "src/formats/rgba_image.h"
#include "src/formats/error.h"
}

class AFToBMPConverter {
private:
    sd_af_file af_file;
    sd_bk_file bk_file;
    bool af_loaded;
    bool bk_loaded;
    
public:
    AFToBMPConverter() : af_loaded(false), bk_loaded(false) {
        sd_af_create(&af_file);
        sd_bk_create(&bk_file);
    }
    
    ~AFToBMPConverter() {
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
        af_loaded = true;
        std::cout << "AF file loaded successfully: " << afPath << std::endl;
        std::cout << "Found " << MAX_AF_MOVES << " move slots" << std::endl;
        return true;
    }
    
    bool loadPalette(const std::string& bkPath) {
        int ret = sd_bk_load(&bk_file, bkPath.c_str());
        if (ret != SD_SUCCESS) {
            std::cerr << "BK file load failed: " << sd_get_error((SD_ERRORCODE)ret) << std::endl;
            return false;
        }
        bk_loaded = true;
        std::cout << "Palette file loaded successfully: " << bkPath << std::endl;
        return true;
    }
    
    bool initSDL() {
        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
            return false;
        }
        return true;
    }
    
    void cleanup() {
        SDL_Quit();
    }
    
    bool convertAllAnimations(const std::string& outputDir) {
        if (!af_loaded || !bk_loaded) {
            std::cerr << "AF or BK file not loaded!" << std::endl;
            return false;
        }
        
        // 출력 디렉토리 생성
        std::filesystem::create_directories(outputDir);
        
        int totalAnimations = 0;
        int totalSprites = 0;
        
        // 모든 모션 검사
        for (int moveIndex = 0; moveIndex < MAX_AF_MOVES; moveIndex++) {
            if (af_file.moves[moveIndex] == nullptr) {
                continue;
            }
            
            sd_move* move = af_file.moves[moveIndex];
            if (move->animation == nullptr) {
                continue;
            }
            
            sd_animation* anim = move->animation;
            if (anim->sprites == nullptr || anim->sprite_count == 0) {
                continue;
            }
            
            std::cout << "\nProcessing Move " << moveIndex << " (Sprites: " << anim->sprite_count << ")" << std::endl;
            
            // 각 스프라이트를 BMP로 변환
            for (int spriteIndex = 0; spriteIndex < anim->sprite_count; spriteIndex++) {
                if (anim->sprites[spriteIndex] == nullptr) {
                    continue;
                }
                
                std::string filename = outputDir + "/move_" + 
                                     std::to_string(moveIndex) + "_sprite_" + 
                                     std::to_string(spriteIndex) + ".bmp";
                
                if (convertSpriteToBMP(anim->sprites[spriteIndex], filename)) {
                    totalSprites++;
                    std::cout << "  Saved: " << filename << std::endl;
                } else {
                    std::cerr << "  Failed to save: " << filename << std::endl;
                }
            }
            
            totalAnimations++;
        }
        
        std::cout << "\n=== Conversion Complete ===" << std::endl;
        std::cout << "Total animations processed: " << totalAnimations << std::endl;
        std::cout << "Total sprites saved: " << totalSprites << std::endl;
        std::cout << "Output directory: " << outputDir << std::endl;
        
        return totalSprites > 0;
    }
    
private:
    bool convertSpriteToBMP(sd_sprite* sprite, const std::string& filename) {
        if (sprite == nullptr || bk_file.palettes[0] == nullptr) {
            return false;
        }
        
        // 스프라이트를 RGBA 이미지로 디코딩
        sd_rgba_image img;
        int ret = sd_sprite_rgba_decode(&img, sprite, bk_file.palettes[0]);
        if (ret != SD_SUCCESS) {
            std::cerr << "Sprite decoding failed: " << sd_get_error((SD_ERRORCODE)ret) << std::endl;
            return false;
        }
        
        // SDL Surface 생성
        SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(
            (void*)img.data, img.w, img.h, 32, img.w * 4,
            0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
        
        if (!surface) {
            std::cerr << "Surface creation failed: " << SDL_GetError() << std::endl;
            sd_rgba_image_free(&img);
            return false;
        }
        
        // BMP 파일로 저장
        int saveResult = SDL_SaveBMP(surface, filename.c_str());
        
        // 메모리 해제
        SDL_FreeSurface(surface);
        sd_rgba_image_free(&img);
        
        return (saveResult == 0);
    }
};

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " <AF_file_path> [BK_file_path] [output_directory]" << std::endl;
    std::cout << "  AF_file_path: Path to the AF file (e.g., FIGHTR0.AF)" << std::endl;
    std::cout << "  BK_file_path: Path to the BK palette file (e.g., ARENA0.BK) [optional]" << std::endl;
    std::cout << "  output_directory: Directory to save BMP files [optional, default: 'output']" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " resources/game_resources/FIGHTR0.AF" << std::endl;
    std::cout << "  " << programName << " resources/game_resources/FIGHTR0.AF resources/game_resources/ARENA0.BK" << std::endl;
    std::cout << "  " << programName << " resources/game_resources/FIGHTR0.AF resources/game_resources/ARENA0.BK jaguar_sprites" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    
    std::string afPath = argv[1];
    std::string bkPath = (argc >= 3) ? argv[2] : "resources/game_resources/ARENA0.BK";
    std::string outputDir = (argc >= 4) ? argv[3] : "output";
    
    std::cout << "=== AF to BMP Converter ===" << std::endl;
    std::cout << "AF file: " << afPath << std::endl;
    std::cout << "BK file: " << bkPath << std::endl;
    std::cout << "Output: " << outputDir << std::endl;
    std::cout << std::endl;
    
    AFToBMPConverter converter;
    
    // SDL 초기화
    if (!converter.initSDL()) {
        return 1;
    }
    
    // AF 파일 로드
    if (!converter.loadAF(afPath)) {
        return 1;
    }
    
    // BK 파일 로드
    if (!converter.loadPalette(bkPath)) {
        return 1;
    }
    
    // 모든 애니메이션을 BMP로 변환
    if (!converter.convertAllAnimations(outputDir)) {
        std::cerr << "Conversion failed!" << std::endl;
        return 1;
    }
    
    std::cout << "\nAll animations converted successfully!" << std::endl;
    return 0;
}
