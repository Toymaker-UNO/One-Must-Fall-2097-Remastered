#ifndef OPENOMF_FORMATS_COLCOORD_HPP
#define OPENOMF_FORMATS_COLCOORD_HPP

#include <cstdint>

namespace openomf {
namespace formats {

/** C formats/colcoord.h sd_coord 대응. 애니메이션 프레임의 충돌 좌표 한 개. */
struct Coord {
    int16_t x = 0;        ///< X position
    int16_t y = 0;        ///< Y position
    uint8_t null = 0;     ///< Padding
    uint8_t frame_id = 0; ///< Sprite the coordinate belongs to
};

} // namespace formats
} // namespace openomf

#endif // OPENOMF_FORMATS_COLCOORD_HPP
