#define SDL_MAIN_HANDLED
// Utils 모듈 빌드/연결 검증: Allocator, Array, Vector, Str, Log, Io 사용.
// Log는 SDL2 mutex 사용 → 테스트에서 SDL_Init/Quit 호출.

#include "hpp/Types.hpp"
#include "../ExternalLibrary/hpp/Sdl2.hpp"
#include "hpp/Allocator.hpp"
#include "hpp/MiscMath.hpp"
#include "hpp/Array.hpp"
#include "hpp/Vector.hpp"
#include "hpp/Str.hpp"
#include "hpp/Log.hpp"
#include "hpp/Io.hpp"

using namespace openomf::utils;

static void test_types_and_allocator()
{
    (void)sizeof(UtilsTypes);
    void* p = omf_malloc(64);
    p = omf_realloc(p, 128);
    omf_free(p);
}

static void test_miscmath()
{
    if (MiscMath::min2(1, 2) != 1) throw 1;
    if (MiscMath::max2(1, 2) != 2) throw 2;
    if (MiscMath::clamp(10, 0, 5) != 5) throw 3;
}

static void test_array()
{
    Array arr;
    arr.array_create();
    arr.array_set(0, reinterpret_cast<const void*>(1));
    if (arr.array_get(0) != reinterpret_cast<void*>(1)) throw 10;
    Iterator it;
    arr.array_iter_begin(it);
    void* v = it.iter_next();
    if (v != reinterpret_cast<void*>(1)) throw 11;
    arr.array_free();
}

static void test_vector()
{
    Vector vec;
    vec.vector_create(sizeof(int));
    int x = 42;
    vec.vector_append(&x);
    if (vec.vector_size() != 1) throw 20;
    int* p = static_cast<int*>(vec.vector_get(0));
    if (p == nullptr || *p != 42) throw 21;
    vec.vector_free();
}

static void test_str()
{
    Str s;
    s.str_create();
    s.str_from_c("hello");
    if (s.str_size() != 5) throw 30;
    if (s.str_equal_c("hello") != true) throw 31;
    s.str_append_c(" world");
    if (s.str_equal_c("hello world") != true) throw 32;
    s.str_free();
}

static void test_log_io()
{
    openomf::external_library::Sdl2::omf_sdl_init(0u);
    Log::log_init();
    Log::log_set_level(LogLevel::INFO);
    Log::log_add_stderr(LogLevel::INFO, false);
    Log::log_msg(LogLevel::INFO, "Utils test running");
    if (Io::file_open("nonexistent_utils.test.main.12345", "r") != nullptr) { /* expect null */ }
    bool exists = Io::file_exists("utils.test.main.cpp");
    Log::log_msg(LogLevel::INFO, "file_exists(utils.test.main.cpp)=%s", exists ? "true" : "false");
    Log::log_close();
    openomf::external_library::Sdl2::omf_sdl_quit();
}

int main()
{
    test_types_and_allocator();
    test_miscmath();
    test_array();
    test_vector();
    test_str();
    test_log_io();
    return 0;
}
