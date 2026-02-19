#ifndef OPENOMF_GAME_SERIAL_HPP
#define OPENOMF_GAME_SERIAL_HPP

#include <cstddef>
#include <cstdint>

namespace openomf {
namespace game {

/** C game/utils/serial.h serial_t. 최소 구조만 정의, 함수는 추후 이식. */
struct Serial {
    size_t len = 0;
    size_t rpos = 0;
    size_t wpos = 0;
    char* data = nullptr;
};

} // namespace game
} // namespace openomf

#endif // OPENOMF_GAME_SERIAL_HPP
