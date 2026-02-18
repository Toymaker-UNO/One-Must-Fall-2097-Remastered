// Audio 모듈 빌드/연결 검증: MusicSource, AudioBackend, NullBackend, Audio
#include "hpp/Audio.hpp"

using namespace openomf::audio;

static void test_audio_backend_scan()
{
    audio_scan_backends();
    if (audio_get_backend_count() < 1) throw 1;
    const char* name = nullptr;
    const char* desc = nullptr;
    if (!audio_get_backend_info(0, &name, &desc)) throw 2;
    if (!name || !desc) throw 3;
}

static void test_audio_init_close()
{
    if (!audio_init(nullptr, 48000, false, 1, 1.0f, 1.0f)) throw 10;
    audio_close();
}

static void test_music_source()
{
    MusicSource src = {};
    music_source_render(&src, nullptr, 0);
    music_source_set_volume(&src, 0.5f);
    music_source_close(&src);
}

int main()
{
    test_audio_backend_scan();
    test_audio_init_close();
    test_music_source();
    return 0;
}
