#ifndef OPENOMF_FORMATS_ERROR_HPP
#define OPENOMF_FORMATS_ERROR_HPP

namespace openomf {
namespace formats {

/** C formats/error.h 대응. */
enum class SdError {
    SUCCESS,
    FILE_OPEN_ERROR,
    FILE_INVALID_TYPE,
    FILE_PARSE_ERROR,
    ANIM_INVALID_STRING,
    OUT_OF_MEMORY,
    INVALID_INPUT,
    FORMAT_NOT_SUPPORTED,
    INVALID_TAG,
    FILE_WRITE_ERROR,
    FILE_READ_ERROR,
    FILE_UNLINK_ERROR,
    FAILURE,
};

} // namespace formats
} // namespace openomf

#endif // OPENOMF_FORMATS_ERROR_HPP
