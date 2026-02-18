// ExternalLibrary 래퍼 헤더 포함 및 기본 동작 확인용 테스트.
// 모든 ExternalLibrary/hpp/*.hpp (Argtable3 제외) include, 타입 존재 여부 등 최소 검사 후 종료.
#define SDL_MAIN_HANDLED
#include "hpp/CStdLib.hpp"
#include "hpp/Enet.hpp"
#include "hpp/Epoxy.hpp"
#include "hpp/LibConfuse.hpp"
#include "hpp/LibPng.hpp"
#include "hpp/LibXmp.hpp"
#include "hpp/NatPmpMiniupnpc.hpp"
#include "hpp/Opusfile.hpp"
#include "hpp/Posix.hpp"
#include "hpp/Sdl2.hpp"
#include "hpp/Sdl2Mixer.hpp"
#include "hpp/WindowsApi.hpp"
#include "hpp/Zlib.hpp"

using namespace openomf::external_library;

/** 래퍼 타입이 존재하고 사용 가능한지 확인 (컴파일 타임 검증). */
static void test_types_exist()
{
    (void)sizeof(Sdl2);
    (void)sizeof(Sdl2Mixer);
    (void)sizeof(Epoxy);
    (void)sizeof(LibPng);
    (void)sizeof(Zlib);
    (void)sizeof(LibConfuse);
    (void)sizeof(Enet);
    (void)sizeof(Opusfile);
    (void)sizeof(LibXmp);
    (void)sizeof(CStdLib);
    (void)sizeof(WindowsApi);
    (void)sizeof(Posix);
    (void)sizeof(NatPmpMiniupnpc);
}

int main()
{
    test_types_exist();
    return 0;
}
