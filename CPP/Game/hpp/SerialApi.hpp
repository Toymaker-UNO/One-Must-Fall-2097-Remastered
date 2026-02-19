#ifndef OPENOMF_GAME_SERIALAPI_HPP
#define OPENOMF_GAME_SERIALAPI_HPP

#include "Serial.hpp"
#include "Utils/hpp/Allocator.hpp"
#include <cstddef>
#include <cstring>

namespace openomf {
namespace game {

constexpr size_t SERIAL_BUF_RESIZE_INC = 64;

inline void serial_create(Serial* s) {
    if (!s) return;
    s->len = SERIAL_BUF_RESIZE_INC;
    s->wpos = 0;
    s->rpos = 0;
    s->data = static_cast<char*>(omf_calloc(s->len, 1));
}

inline void serial_free(Serial* s) {
    if (!s) return;
    omf_free(s->data);
    s->data = nullptr;
    s->len = 0;
    s->wpos = s->rpos = 0;
}

} // namespace game
} // namespace openomf

#endif // OPENOMF_GAME_SERIALAPI_HPP
