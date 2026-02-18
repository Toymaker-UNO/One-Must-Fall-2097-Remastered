#ifndef OPENOMF_UTILS_MISCMATH_HPP
#define OPENOMF_UTILS_MISCMATH_HPP

#include <cmath>
#include <cstddef>
#include <limits>

namespace openomf {
namespace utils {

/** C utils/miscmath.h 대응 유틸리티 (헤더 전용). */
struct MiscMath {
    static constexpr float MATH_PI = 3.14159265f;

    static int clamp(int a_val, int a_min, int a_max) {
        if (a_val > a_max) return a_max;
        if (a_val < a_min) return a_min;
        return a_val;
    }

    static float clampf(float a_val, float a_min, float a_max) {
        if (a_val > a_max) return a_max;
        if (a_val < a_min) return a_min;
        return a_val;
    }

    static int clamp_long_to_int(long a_val) {
        if (a_val > std::numeric_limits<int>::max()) return std::numeric_limits<int>::max();
        if (a_val < std::numeric_limits<int>::min()) return std::numeric_limits<int>::min();
        return static_cast<int>(a_val);
    }

    static int max3(int a, int b, int c) {
        int m = a;
        if (b > m) m = b;
        if (c > m) m = c;
        return m;
    }

    static int max2(int a, int b) { return (a > b) ? a : b; }
    static int min2(int a, int b) { return (a > b) ? b : a; }
    static unsigned umax2(unsigned a, unsigned b) { return (a > b) ? a : b; }
    static unsigned umin2(unsigned a, unsigned b) { return (a > b) ? b : a; }
    static unsigned udist(unsigned a, unsigned b) { return (a > b) ? a - b : b - a; }

    static float dist(float a, float b) {
        return std::fabs((a < b ? a : b) - (a > b ? a : b)) * (a < b ? 1.0f : -1.0f);
    }

    static size_t smin2(size_t a, size_t b) { return (a > b) ? b : a; }

    static unsigned powu(unsigned x, unsigned y) {
        unsigned ret = 1;
        while (y--) ret *= x;
        return ret;
    }
};

}  // namespace utils
}  // namespace openomf

#endif
