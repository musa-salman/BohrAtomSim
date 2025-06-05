#ifndef EOM_UTILS_HPP
#define EOM_UTILS_HPP

#include <math.h>
#include <sleef.h>

#include "utils/types.h"

#if defined(__GNUC__) || defined(__clang__) || defined(__INTEL_COMPILER)
#define SIM_CONST __attribute__((const))
#define SIM_INLINE __attribute__((always_inline))
#elif defined(_MSC_VER)
#define SIM_CONST
#define SIM_INLINE __forceinline
#endif

#define SPEED_OF_LIGHT_SQUARE FLOAT_LITERAL_SUFFIX(18778.86507043055614177)
#define INV_SPEED_OF_LIGHT_SQUARED                                             \
    FLOAT_LITERAL_SUFFIX(0.00005325135444817764497014)

#define G_FACTOR FLOAT_LITERAL_SUFFIX(-2.002319304)
#define S_Z_FACTOR FLOAT_LITERAL_SUFFIX(0.5)

#define HALF_PI FLOAT_LITERAL_SUFFIX(1.570796326794896557998981)
#define SIM_PI FLOAT_LITERAL_SUFFIX(3.14159265358979323846)
#define TWO_PI FLOAT_LITERAL_SUFFIX(6.28318530717958647692)

namespace eom {

struct Vector3;

struct Vector2 {
    scalar x;
    scalar y;

    SIM_CONST SIM_INLINE inline scalar magnitude() const noexcept {
        return sqrt(x * x + y * y);
    }

    SIM_CONST SIM_INLINE inline scalar
    dot(const Vector2 &other) const noexcept {
        return x * other.x + y * other.y;
    }

    SIM_CONST SIM_INLINE inline scalar
    angle_with(const Vector2 &other) const noexcept {
        scalar dot_product = dot(other);
        scalar magnitudes = magnitude() * other.magnitude();
        return (magnitudes > 0) ? acos(dot_product / magnitudes) : 0;
    }
};

struct radial_bounds {
    scalar r_min;
    scalar r_max;
};

radial_bounds compute_radial_limits(quantum_principle principal,
                                    quantum_angular angular);

Vector3 spherical_to_cartesian(scalar radius, scalar theta, scalar phi);

scalar compute_angular_distance(const Vector3 &v1, const Vector3 &v2);

SIM_CONST SIM_INLINE inline float cos(float x) noexcept {
    return Sleef_cosf1_u35purecfma(x);
}

SIM_CONST SIM_INLINE inline double cos(double x) noexcept {
    return Sleef_cosd1_u35purecfma(x);
}

SIM_CONST SIM_INLINE inline long double cos(long double x) noexcept {
    const long double inv_pi = 1.0L / M_PIl;
    const Sleef_longdouble2 result = Sleef_sincospil_u35(x * inv_pi);
    return result.y;
}

SIM_CONST SIM_INLINE inline float sin(float x) noexcept {
    return Sleef_sinf1_u35purecfma(x);
}

SIM_CONST SIM_INLINE inline double sin(double x) noexcept {
    return Sleef_finz_sind1_u35purecfma(x);
}

SIM_CONST SIM_INLINE inline long double sin(long double x) noexcept {
    const long double inv_pi = 1.0L / M_PIl;
    const Sleef_longdouble2 result = Sleef_sincospil_u35(x * inv_pi);
    return result.x;
}

SIM_CONST SIM_INLINE inline float sqrt(float x) noexcept {
    return Sleef_sqrtf1_u35(x);
}

SIM_CONST SIM_INLINE inline double sqrt(double x) noexcept {
    return Sleef_sqrtd1_u35(x);
}

SIM_CONST SIM_INLINE inline long double sqrt(long double x) noexcept {
    return (long double)Sleef_sqrtd1_u35((double)x);
}

SIM_CONST SIM_INLINE inline bool isLocalMax(scalar prevVarDot,
                                            scalar currVarDot) noexcept {
    return prevVarDot < 0 && currVarDot > 0;
}

#define SQUARE(x) ((x) * (x))

struct Vector3 {
    scalar x;
    scalar y;
    scalar z;

    SIM_CONST SIM_INLINE inline scalar magnitude() const noexcept {
        return sqrt(x * x + y * y + z * z);
    }

    SIM_CONST SIM_INLINE inline scalar
    dot(const Vector3 &other) const noexcept {
        return x * other.x + y * other.y + z * other.z;
    }

    SIM_CONST SIM_INLINE inline scalar
    angle_with(const Vector3 &other) const noexcept {
        scalar dot_product = dot(other);
        scalar magnitudes = magnitude() * other.magnitude();
        return (magnitudes > 0) ? acos(dot_product / magnitudes) : 0;
    }

    SIM_CONST SIM_INLINE inline Vector3 normalized() const noexcept {
        scalar mag = magnitude();
        return (mag > 0) ? Vector3{x / mag, y / mag, z / mag}
                         : Vector3{0, 0, 0};
    }

    SIM_CONST SIM_INLINE inline Vector3 operator*(scalar s) const {
        return Vector3{x * s, y * s, z * s};
    }
};

} // namespace eom

#endif // EOM_UTILS_HPP
