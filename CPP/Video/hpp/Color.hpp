#ifndef OPENOMF_VIDEO_COLOR_HPP
#define OPENOMF_VIDEO_COLOR_HPP

#include <cstdint>

namespace openomf {
namespace video {

// C color.h: palette index constants (FIXME: set properly; currently green-ish)
constexpr uint8_t COLOR_RED        = 0xA7;
constexpr uint8_t COLOR_BLUE      = 0xA7;
constexpr uint8_t COLOR_LIGHT_BLUE = 0xA7;
constexpr uint8_t COLOR_BLACK     = 0xA7;
constexpr uint8_t COLOR_GREEN     = 0xA7;
constexpr uint8_t COLOR_DARK_GREEN = 0xA0;
constexpr uint8_t COLOR_YELLOW    = 0xCF;

/** C color.h color struct */
struct Color {
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
};

} // namespace video
} // namespace openomf

#endif // OPENOMF_VIDEO_COLOR_HPP
