#ifndef OPENOMF_EXTERNALLIBRARY_SDL2_MIXER_HPP
#define OPENOMF_EXTERNALLIBRARY_SDL2_MIXER_HPP

#include <SDL2/SDL_mixer.h>

namespace openomf {
namespace external_library {

/** SDL2_mixer 라이브러리 정적 래퍼 (오디오 재생). */
class Sdl2Mixer {
public:
    /** Mixer 하위 시스템 초기화. */
    static int omf_mix_init(int a_flags) { return Mix_Init(a_flags); }

    /** Mixer 종료. */
    static void omf_mix_quit() { Mix_Quit(); }

    /** 오디오 장치 열기. */
    static int omf_mix_open_audio_device(int a_frequency, Uint16 a_format, int a_channels, int a_chunksize,
                                         const char* a_device, int a_allowed_changes) {
        return Mix_OpenAudioDevice(a_frequency, a_format, a_channels, a_chunksize, a_device, a_allowed_changes);
    }

    /** 오디오 장치 닫기. */
    static void omf_mix_close_audio() { Mix_CloseAudio(); }

    /** 현재 오디오 장치 포맷 정보 조회. */
    static int omf_mix_query_spec(int* a_frequency, Uint16* a_format, int* a_channels) {
        return Mix_QuerySpec(a_frequency, a_format, a_channels);
    }

    /** 재생 채널 개수 할당. */
    static int omf_mix_allocate_channels(int a_numchans) { return Mix_AllocateChannels(a_numchans); }

    /** 채널 또는 전체 볼륨 설정. */
    static int omf_mix_volume(int a_channel, int a_volume) { return Mix_Volume(a_channel, a_volume); }

    /** 그룹에서 재생 가능한 채널 하나 반환. */
    static int omf_mix_group_available(int a_tag) { return Mix_GroupAvailable(a_tag); }

    /** 채널 스테레오 파닝 설정. */
    static int omf_mix_set_panning(int a_channel, Uint8 a_left, Uint8 a_right) {
        return Mix_SetPanning(a_channel, a_left, a_right);
    }

    /** 채널에서 페이드 인하며 샘플 재생. */
    static int omf_mix_fade_in_channel_timed(int a_channel, Mix_Chunk* a_chunk, int a_loops, int a_ms, int a_ticks) {
        return Mix_FadeInChannelTimed(a_channel, a_chunk, a_loops, a_ms, a_ticks);
    }

    /** 채널 페이드 아웃. */
    static int omf_mix_fade_out_channel(int a_channel, int a_ms) { return Mix_FadeOutChannel(a_channel, a_ms); }

    /** 재생 중인 뮤직 정지. */
    static void omf_mix_halt_music() { Mix_HaltMusic(); }

    /** 뮤직 재생 후처리 훅 등록. */
    static void omf_mix_hook_music(void (*a_func)(void* a_udata, Uint8* a_stream, int a_len), void* a_arg) {
        Mix_HookMusic(a_func, a_arg);
    }

    /** 채널 재생 완료 시 호출될 콜백 설정. */
    static void omf_mix_channel_finished(void (*a_channel_finished)(int a_channel)) {
        Mix_ChannelFinished(a_channel_finished);
    }

    /** 마지막 Mixer 오류 메시지 반환. */
    static const char* omf_mix_get_error() { return Mix_GetError(); }
};

} // namespace external_library
} // namespace openomf

#endif // OPENOMF_EXTERNALLIBRARY_SDL2_MIXER_HPP
