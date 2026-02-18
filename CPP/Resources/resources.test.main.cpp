// Resources 모듈 빌드/연결 검증: Ids, PathManager, BkLoader, AfLoader
#include <cstdio>
#include <string>
#include "hpp/Ids.hpp"
#include "hpp/PathManager.hpp"
#include "hpp/BkLoader.hpp"
#include "hpp/AfLoader.hpp"

using namespace openomf::resources;

static void test_ids()
{
    if (get_resource_file(static_cast<unsigned int>(ResourceId::BK_MENU)) == nullptr) throw 1;
    if (get_resource_file(static_cast<unsigned int>(ResourceId::AF_JAGUAR)) == nullptr) throw 2;
    if (std::string(get_resource_file(static_cast<unsigned int>(ResourceId::BK_MENU))) != "MAIN.BK") throw 3;
    if (get_resource_name(static_cast<unsigned int>(ResourceId::BK_ARENA0)) == nullptr) throw 4;
    if (!is_arena(static_cast<unsigned int>(ResourceId::BK_ARENA2))) throw 5;
    if (!is_har(static_cast<unsigned int>(ResourceId::AF_NOVA))) throw 6;
    if (!is_music(static_cast<unsigned int>(ResourceId::PSM_MENU))) throw 7;
}

static void test_pathmanager()
{
    PathManager::set_resource_base(".");
    const char* p = PathManager::get_resource_path(static_cast<unsigned int>(ResourceId::BK_MENU));
    if (!p) throw 10;
    PathManager::set_resource_base("res");
    p = PathManager::get_resource_path(static_cast<unsigned int>(ResourceId::BK_MENU));
    if (!p) throw 11;
    std::string s(p);
    if (s.find("MAIN.BK") == std::string::npos) throw 12;
}

static void test_bk_loader()
{
    openomf::formats::BkFile bk = {};
    openomf::formats::SdError err = load_bk_file("nonexistent.bk", &bk);
    if (err == openomf::formats::SdError::SUCCESS) {
        openomf::formats::bk_free(&bk);
        throw 20;
    }
}

static void test_af_loader()
{
    PathManager::set_resource_base(".");
    openomf::formats::AfFile af = {};
    openomf::formats::SdError err = load_af_file(static_cast<unsigned int>(ResourceId::AF_JAGUAR), &af);
    if (err == openomf::formats::SdError::SUCCESS)
        openomf::formats::af_free(&af);
}

int main()
{
    test_ids();
    test_pathmanager();
    test_bk_loader();
    test_af_loader();
    return 0;
}
