#ifndef OPENOMF_AUDIO_AUDIO_HPP
#define OPENOMF_AUDIO_AUDIO_HPP

#include "hpp/AudioBackend.hpp"
#include "hpp/MusicSource.hpp"
#include "hpp/NullBackend.hpp"
#include <cstring>
#include <cstddef>

namespace openomf {
namespace audio {

constexpr int MAX_AVAILABLE_BACKENDS = 8;

struct AvailableBackend {
    void (*set_callbacks)(AudioBackend* b) = nullptr;
    const char* name = nullptr;
    const char* description = nullptr;
};

inline static AvailableBackend s_available[MAX_AVAILABLE_BACKENDS];
inline static int s_backend_count = 0;
inline static AudioBackend s_current = {};

inline void audio_scan_backends() {
    s_backend_count = 0;
    AudioBackend tmp = {};
    null_audio_backend_set_callbacks(&tmp);
    if (tmp.is_available && tmp.is_available()) {
        s_available[s_backend_count].set_callbacks = null_audio_backend_set_callbacks;
        s_available[s_backend_count].name = tmp.get_name ? tmp.get_name() : "";
        s_available[s_backend_count].description = tmp.get_description ? tmp.get_description() : "";
        s_backend_count++;
    }
}

inline int audio_get_backend_count() {
    return s_backend_count;
}

inline bool audio_get_backend_info(int index, const char** name, const char** description) {
    if (index < 0 || index >= s_backend_count) return false;
    if (name) *name = s_available[index].name;
    if (description) *description = s_available[index].description;
    return true;
}

inline bool audio_find_backend(const char* try_name) {
    if (try_name && *try_name != '\0') {
        for (int i = 0; i < s_backend_count; i++) {
            if (s_available[i].name && std::strcmp(s_available[i].name, try_name) == 0) {
                s_available[i].set_callbacks(&s_current);
                return true;
            }
        }
    }
    if (s_backend_count > 0) {
        s_available[0].set_callbacks(&s_current);
        return true;
    }
    std::memset(&s_current, 0, sizeof(AudioBackend));
    return false;
}

/** C audio_init */
inline bool audio_init(const char* try_name, int sample_rate, bool mono, int resampler, float music_volume, float sound_volume) {
    (void)sample_rate;
    (void)mono;
    (void)resampler;
    (void)music_volume;
    (void)sound_volume;
    if (!audio_find_backend(try_name)) return false;
    if (s_current.create) s_current.create(&s_current);
    if (s_current.setup_context && !s_current.setup_context(s_current.ctx, static_cast<unsigned>(sample_rate), mono, resampler, music_volume, sound_volume)) {
        if (s_current.destroy) s_current.destroy(&s_current);
        return false;
    }
    return true;
}

/** C audio_close */
inline void audio_close() {
    if (s_current.close_context) s_current.close_context(s_current.ctx);
    if (s_current.destroy) s_current.destroy(&s_current);
    std::memset(&s_current, 0, sizeof(AudioBackend));
}

/** C audio_play_sound. id로 샘플 재생. (sounds_loader 연동 전에는 버퍼 없이 no-op.) */
inline int audio_play_sound(int id, float volume, float panning, int pitch) {
    (void)id;
    if (s_current.play_sound)
        return s_current.play_sound(s_current.ctx, nullptr, 0, 44100, volume, panning, pitch, 0);
    return -1;
}

/** C audio_play_music. resource_id(PSM_* 등)로 BGM 재생. (로더 연동 전에는 no-op.) */
inline void audio_play_music(unsigned int resource_id) {
    (void)resource_id;
    if (s_current.play_music) {
        MusicSource src = {};
        s_current.play_music(s_current.ctx, &src);
    }
}

} // namespace audio
} // namespace openomf

#endif // OPENOMF_AUDIO_AUDIO_HPP
