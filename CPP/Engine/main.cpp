/**
 * OMF C++ 진입점.
 * C의 main.c 대체. pm_init → (설정/로그) → SDL_Init → engine_init → engine_run → engine_close → SDL_Quit.
 * (omf_config.json, settings_init, log_init은 추후 구현)
 */
#include "hpp/Engine.hpp"
#include "ExternalLibrary/hpp/Sdl2.hpp"
#include "Resources/hpp/PathManager.hpp"
#include <cstring>
#include <string>

int main(int /*argc*/, char** /*argv*/) {
    openomf::engine::EngineInitFlags init_flags;
    std::memset(&init_flags, 0, sizeof(init_flags));

    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) != 0)
        return 1;

    /* C pathmanager: 리소스는 base + "resources/" 에서 로드 (pm_in_release_mode 시 RESOURCE_PATH) */
    char* base = SDL_GetBasePath();
    std::string resource_base = base ? base : ".";
    if (!resource_base.empty() && resource_base.back() != '/' && resource_base.back() != '\\')
        resource_base += openomf::resources::PathManager::path_sep;
#if defined(_WIN32) || defined(WIN32)
    resource_base += "resources\\";
#else
    resource_base += "resources/";
#endif
    openomf::resources::PathManager::set_resource_base(resource_base.c_str());
    if (base)
        SDL_free(base);

    if (openomf::engine::engine_init(&init_flags) != 0) {
        SDL_Quit();
        return 1;
    }

    openomf::engine::engine_run(&init_flags);
    openomf::engine::engine_close();
    SDL_Quit();
    return 0;
}
