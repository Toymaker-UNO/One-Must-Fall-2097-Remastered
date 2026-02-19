#include "hpp/GameState.hpp"
#include "hpp/CommonDefines.hpp"
#include "hpp/FightStats.hpp"
#include "hpp/Scene.hpp"
#include "hpp/Object.hpp"
#include "hpp/Intersect.hpp"
#include "hpp/ObjectApi.hpp"
#include "hpp/SceneApi.hpp"
#include "hpp/Arena.hpp"
#include "hpp/ArenaConstraints.hpp"
#include "hpp/Har.hpp"
#include "Utils/hpp/Vec.hpp"

using namespace openomf::game;

static void test_game_state_defaults()
{
    GameState gs{};
    if (gs.run != 0) throw 1;
    if (gs.paused != 0) throw 2;
    if (gs.tick != 0) throw 3;
    if (gs.match_settings.rounds != 0) throw 4;
}

static void test_common_defines()
{
    if (!is_valid_scene(SCENE_MENU)) throw 10;
    if (is_valid_scene(999)) throw 11;
}

static void test_fight_stats()
{
    FightStats fs{};
    if (fs.winner != 0) throw 20;
    if (fs.finish != FightFinisher::FINISH_NONE) throw 21;
}

static void test_scene_and_object_protos()
{
    Scene sc{};
    sc.id = SCENE_ARENA0;
    if (sc.bk_data != nullptr) throw 30;
    if (sc.af_data[0] != nullptr) throw 31;

    Object obj{};
    obj.id = 1;
    if (obj.gs != nullptr) throw 32;
    if (obj.cur_animation != nullptr) throw 33;
}

static void test_object_api()
{
    GameState gs{};
    Object obj{};
    openomf::utils::Vec2i pos = openomf::utils::vec2i_create(10, 20);
    openomf::utils::Vec2f vel = openomf::utils::vec2f_create(0.f, 0.f);
    object_create(&obj, &gs, pos, vel);
    if (obj.gs != &gs) throw 40;
    if (obj.id == 0) throw 41;
    if (obj.pos.x != 10.f || obj.pos.y != 20.f) throw 42;
    object_set_pos(&obj, openomf::utils::vec2i_create(5, 5));
    openomf::utils::Vec2i p = object_get_pos(&obj);
    if (p.x != 5 || p.y != 5) throw 43;
    object_free(&obj);
}

static void test_scene_api()
{
    GameState gs{};
    Scene sc{};
    if (scene_create_empty(&sc, &gs, SCENE_NONE) != 1) throw 50;
    if (scene_create_empty(&sc, &gs, SCENE_ARENA0) != 0) throw 51;
    if (sc.id != SCENE_ARENA0 || sc.gs != &gs) throw 52;
    if (!scene_is_arena(&sc)) throw 53;
    scene_free(&sc);
}

static void test_arena_and_constraints()
{
    if (ARENA_LEFT_WALL != 20 || ARENA_FLOOR != 190) throw 60;
    Scene sc{};
    GameState gs{};
    scene_create_empty(&sc, &gs, SCENE_ARENA0);
    if (arena_create(&sc) != 0) throw 61;
    if (arena_get_state(&sc) != ARENA_STATE_STARTING) throw 62;
    arena_set_state(&sc, ARENA_STATE_FIGHTING);
    scene_free(&sc);
}

int main()
{
    test_game_state_defaults();
    test_common_defines();
    test_fight_stats();
    test_scene_and_object_protos();
    test_object_api();
    test_scene_api();
    test_arena_and_constraints();
    return 0;
}

