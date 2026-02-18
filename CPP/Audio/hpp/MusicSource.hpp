#ifndef OPENOMF_AUDIO_MUSICSOURCE_HPP
#define OPENOMF_AUDIO_MUSICSOURCE_HPP

namespace openomf {
namespace audio {

/** C audio/sources/music_source.h music_render 등 함수 포인터 타입 */
using MusicRender = void (*)(void* ctx, char* data, int len);
using MusicSetVolume = void (*)(void* ctx, float volume);
using MusicClose = void (*)(void* ctx);

/** C music_resampler */
struct MusicResampler {
    int internal_id = 0;
    bool is_default = false;
    const char* name = nullptr;
};

/** C music_source */
struct MusicSource {
    void* context = nullptr;
    MusicRender render = nullptr;
    MusicSetVolume set_volume = nullptr;
    MusicClose close = nullptr;
};

inline void music_source_render(MusicSource* src, char* data, int len) {
    if (src && src->context && src->render)
        src->render(src->context, data, len);
}

inline void music_source_set_volume(MusicSource* src, float volume) {
    if (src && src->context && src->set_volume)
        src->set_volume(src->context, volume);
}

inline void music_source_close(MusicSource* src) {
    if (src && src->context && src->close) {
        src->close(src->context);
        src->context = nullptr;
    }
}

} // namespace audio
} // namespace openomf

#endif // OPENOMF_AUDIO_MUSICSOURCE_HPP
