#ifndef OPENOMF_AUDIO_AUDIOBACKEND_HPP
#define OPENOMF_AUDIO_AUDIOBACKEND_HPP

#include "hpp/MusicSource.hpp"
#include <cstddef>

namespace openomf {
namespace audio {

constexpr float VOLUME_DEFAULT = 1.0f;
constexpr float PANNING_DEFAULT = 0.0f;
constexpr float VOLUME_MAX = 1.0f;
constexpr float PANNING_MAX = 1.0f;
constexpr float VOLUME_MIN = 0.0f;
constexpr float PANNING_MIN = -1.0f;

/** C audio_sample_rate */
struct AudioSampleRate {
    unsigned int sample_rate = 0;
    bool is_default = false;
    const char* name = nullptr;
};

struct AudioBackend;

using IsBackendAvailableFn = bool (*)(void);
using GetBackendDescriptionFn = const char* (*)(void);
using GetBackendNameFn = const char* (*)(void);
using GetBackendSampleRatesFn = unsigned int (*)(const AudioSampleRate** sample_rates);
using GetBackendInfoFn = void (*)(void* ctx, unsigned* sample_rate, unsigned* channels, unsigned* resampler);
using CreateBackendFn = void (*)(AudioBackend* backend);
using DestroyBackendFn = void (*)(AudioBackend* backend);
using SetBackendSoundVolumeFn = void (*)(void* ctx, float volume);
using SetBackendMusicVolumeFn = void (*)(void* ctx, float volume);
using SetupBackendContextFn = bool (*)(void* ctx, unsigned sample_rate, bool mono, int resampler, float music_volume, float sound_volume);
using CloseBackendContextFn = void (*)(void* ctx);
using PlaySoundFn = int (*)(void* ctx, const char* buf, size_t len, int freq, float volume, float panning, int pitch, int fade);
using PlayMusicFn = void (*)(void* ctx, const MusicSource* src);
using StopMusicFn = void (*)(void* ctx);
using FadeOutFn = void (*)(int playback_id, int ms);

/** C audio_backend */
struct AudioBackend {
    IsBackendAvailableFn is_available = nullptr;
    GetBackendDescriptionFn get_description = nullptr;
    GetBackendNameFn get_name = nullptr;
    GetBackendSampleRatesFn get_sample_rates = nullptr;
    GetBackendInfoFn get_info = nullptr;
    SetBackendSoundVolumeFn set_sound_volume = nullptr;
    SetBackendMusicVolumeFn set_music_volume = nullptr;
    CreateBackendFn create = nullptr;
    DestroyBackendFn destroy = nullptr;
    SetupBackendContextFn setup_context = nullptr;
    CloseBackendContextFn close_context = nullptr;
    PlaySoundFn play_sound = nullptr;
    PlayMusicFn play_music = nullptr;
    StopMusicFn stop_music = nullptr;
    FadeOutFn fade_out = nullptr;
    void* ctx = nullptr;
};

} // namespace audio
} // namespace openomf

#endif // OPENOMF_AUDIO_AUDIOBACKEND_HPP
