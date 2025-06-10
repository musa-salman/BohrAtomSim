#pragma once

#include "Vector2.hpp"
#include "Vector3.hpp"
#include "intrinsics.hpp"
#include "macros.hpp"

namespace physics::math {

SIM_CONST SIM_INLINE scalar magnitude(const Vector2 &v) noexcept {
    return sqrt(v.x * v.x + v.y * v.y);
}

SIM_CONST SIM_INLINE scalar dot(const Vector2 &a, const Vector2 &b) noexcept {
    return a.x * b.x + a.y * b.y;
}

SIM_CONST SIM_INLINE scalar angle_with(const Vector2 &a,
                                       const Vector2 &b) noexcept {
    scalar d = dot(a, b);
    scalar m = magnitude(a) * magnitude(b);
    return (m > 0) ? acos(d / m) : 0;
}

SIM_CONST SIM_INLINE scalar computeMagnitude(const Vector3 &v) noexcept {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

SIM_CONST SIM_INLINE scalar dot(const Vector3 &a, const Vector3 &b) noexcept {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

SIM_CONST SIM_INLINE scalar computeAngleBetween(const Vector3 &a,
                                                const Vector3 &b) noexcept {
    scalar d = dot(a, b);
    scalar m = computeMagnitude(a) * computeMagnitude(b);
    return (m > 0) ? acos(d / m) : 0;
}

SIM_CONST SIM_INLINE Vector3 normalized(const Vector3 &v) noexcept {
    scalar mag = computeMagnitude(v);
    return (mag > 0) ? Vector3{v.x / mag, v.y / mag, v.z / mag}
                     : Vector3{0, 0, 0};
}

SIM_CONST SIM_INLINE Vector3 operator*(const Vector3 &v, scalar s) {
    return Vector3{v.x * s, v.y * s, v.z * s};
}

} // namespace physics::math
