#ifndef OPENOMF_UTILS_VEC_HPP
#define OPENOMF_UTILS_VEC_HPP

#include <cmath>

namespace openomf {
namespace utils {

/** C utils/vec.h vec2i 대응. */
struct Vec2i {
    int x = 0;
    int y = 0;
};

/** C utils/vec.h vec2f 대응. */
struct Vec2f {
    float x = 0.f;
    float y = 0.f;
};

inline Vec2i vec2i_create(int x, int y) {
    Vec2i v;
    v.x = x;
    v.y = y;
    return v;
}

inline Vec2f vec2f_create(float x, float y) {
    Vec2f v;
    v.x = x;
    v.y = y;
    return v;
}

inline Vec2i vec2i_add(Vec2i a, Vec2i b) {
    a.x += b.x;
    a.y += b.y;
    return a;
}

inline Vec2i vec2i_sub(Vec2i a, Vec2i b) {
    a.x -= b.x;
    a.y -= b.y;
    return a;
}

inline Vec2f vec2f_add(Vec2f a, Vec2f b) {
    a.x += b.x;
    a.y += b.y;
    return a;
}

inline Vec2f vec2f_sub(Vec2f a, Vec2f b) {
    a.x -= b.x;
    a.y -= b.y;
    return a;
}

inline Vec2i vec2f_to_i(Vec2f f) {
    Vec2i i;
    i.x = static_cast<int>(f.x);
    i.y = static_cast<int>(f.y);
    return i;
}

inline Vec2f vec2i_to_f(Vec2i i) {
    Vec2f f;
    f.x = static_cast<float>(i.x);
    f.y = static_cast<float>(i.y);
    return f;
}

inline float vec2f_mag(Vec2f a) {
    return std::sqrt(a.x * a.x + a.y * a.y);
}

inline Vec2f vec2f_norm(Vec2f a) {
    float mag = vec2f_mag(a);
    if (mag > 0.f) {
        a.x /= mag;
        a.y /= mag;
    }
    return a;
}

inline float vec2f_dist(Vec2f a, Vec2f b) {
    return vec2f_mag(vec2f_sub(a, b));
}

} // namespace utils
} // namespace openomf

#endif // OPENOMF_UTILS_VEC_HPP
