#ifndef OPENOMF_AUDIO_NULLBACKEND_HPP
#define OPENOMF_AUDIO_NULLBACKEND_HPP

#include "hpp/AudioBackend.hpp"

namespace openomf {
namespace audio {

namespace detail {
static const AudioSampleRate supported_sample_rates[] = { { 48000, true, "48000Hz" } };
static constexpr unsigned int supported_sample_rate_count = 1;

static bool null_is_available(void) { return true; }
static const char* null_get_description(void) { return "NULL audio output"; }
static const char* null_get_name(void) { return "NULL"; }
static unsigned int null_get_sample_rates(const AudioSampleRate** sample_rates) {
    *sample_rates = supported_sample_rates;
    return supported_sample_rate_count;
}
static void null_get_info(void* ctx, unsigned* sample_rate, unsigned* channels, unsigned* resampler) {
    (void)ctx;
    if (sample_rate) *sample_rate = 48000;
    if (channels) *channels = 2;
    if (resampler) *resampler = 1;
}
static void null_create(AudioBackend*) {}
static void null_destroy(AudioBackend*) {}
static void null_set_sound_volume(void*, float) {}
static void null_set_music_volume(void*, float) {}
static bool null_setup_context(void*, unsigned, bool, int, float, float) { return true; }
static void null_close_context(void*) {}
static int null_play_sound(void*, const char*, size_t, int, float, float, int, int) { return -1; }
static void null_play_music(void* userdata, const MusicSource* src) {
    (void)userdata;
    if (src) {
        MusicSource dst = *src;
        music_source_close(&dst);
    }
}
static void null_stop_music(void*) {}
static void null_fade_out(int, int) {}
}

inline void null_audio_backend_set_callbacks(AudioBackend* b) {
    if (!b) return;
    b->is_available = detail::null_is_available;
    b->get_description = detail::null_get_description;
    b->get_name = detail::null_get_name;
    b->get_sample_rates = detail::null_get_sample_rates;
    b->get_info = detail::null_get_info;
    b->create = detail::null_create;
    b->destroy = detail::null_destroy;
    b->set_sound_volume = detail::null_set_sound_volume;
    b->set_music_volume = detail::null_set_music_volume;
    b->setup_context = detail::null_setup_context;
    b->close_context = detail::null_close_context;
    b->play_sound = detail::null_play_sound;
    b->play_music = detail::null_play_music;
    b->stop_music = detail::null_stop_music;
    b->fade_out = detail::null_fade_out;
    b->ctx = nullptr;
}

} // namespace audio
} // namespace openomf

#endif
