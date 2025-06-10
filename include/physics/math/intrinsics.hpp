#pragma once

#include <cmath>
#include <sleef.h>

#include "macros.hpp"

namespace physics::math {

SIM_CONST SIM_INLINE float cos(float x) noexcept {
    return Sleef_cosf1_u35purecfma(x);
}

SIM_CONST SIM_INLINE double cos(double x) noexcept {
    return Sleef_cosd1_u35purecfma(x);
}

SIM_CONST SIM_INLINE long double cos(long double x) noexcept {
    const long double inv_pi = 1.0L / M_PIl;
    const Sleef_longdouble2 result = Sleef_sincospil_u35(x * inv_pi);
    return result.y;
}

SIM_CONST SIM_INLINE float sin(float x) noexcept {
    return Sleef_sinf1_u35purecfma(x);
}

SIM_CONST SIM_INLINE double sin(double x) noexcept {
    return Sleef_finz_sind1_u35purecfma(x);
}

SIM_CONST SIM_INLINE long double sin(long double x) noexcept {
    const long double inv_pi = 1.0L / M_PIl;
    const Sleef_longdouble2 result = Sleef_sincospil_u35(x * inv_pi);
    return result.x;
}

SIM_CONST SIM_INLINE float sqrt(float x) noexcept {
    return Sleef_sqrtf1_u35(x);
}

SIM_CONST SIM_INLINE double sqrt(double x) noexcept {
    return Sleef_sqrtd1_u35(x);
}

SIM_CONST SIM_INLINE long double sqrt(long double x) noexcept {
    return static_cast<long double>(Sleef_sqrtd1_u35(static_cast<double>(x)));
}

} // namespace physics::math