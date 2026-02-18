#ifndef OPENOMF_EXTERNALLIBRARY_OPUSFILE_HPP
#define OPENOMF_EXTERNALLIBRARY_OPUSFILE_HPP

#include <opus/opusfile.h>

namespace openomf {
namespace external_library {

/** opusfile(Opus) 라이브러리 정적 래퍼 (Opus 오디오 디코딩). */
class Opusfile {
public:
    /** 파일 경로로 Opus 파일 열기. */
    static OggOpusFile* omf_op_open_file(const char* a_path, int* a_error) {
        return op_open_file(a_path, a_error);
    }

    /** OggOpusFile 핸들 해제. */
    static void omf_op_free(OggOpusFile* a_of) { op_free(a_of); }

    /** 스테레오 PCM으로 디코딩하여 읽기. */
    static int omf_op_read_stereo(OggOpusFile* a_of, opus_int16* a_pcm, int a_buf_size) {
        return op_read_stereo(a_of, a_pcm, a_buf_size);
    }

    /** 바이트 오프셋으로 시크. */
    static int omf_op_raw_seek(OggOpusFile* a_of, opus_int64 a_byte_offset) {
        return op_raw_seek(a_of, a_byte_offset);
    }
};

} // namespace external_library
} // namespace openomf

#endif // OPENOMF_EXTERNALLIBRARY_OPUSFILE_HPP
