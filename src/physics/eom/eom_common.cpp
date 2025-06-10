
#include "physics/eom/eom_common.hpp"
#include "physics/math/intrinsics.hpp"

namespace physics::eom {
math::Vector3 sphericalToCartesian(math::scalar radius, math::scalar theta,
                                   math::scalar phi) {
    math::Vector3 result;

    result.x = radius * math::sin(theta) * math::cos(phi);
    result.y = radius * math::sin(theta) * math::sin(phi);
    result.z = radius * math::cos(theta);

    return result;
}

math::scalar computeAngularDistance(const math::Vector3 &v1,
                                    const math::Vector3 &v2) {
    const math::scalar dot = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    const math::scalar cross_mag = math::sqrt(
        SQUARE(v1.y * v2.z - v1.z * v2.y) + SQUARE(v1.z * v2.x - v1.x * v2.z) +
        SQUARE(v1.x * v2.y - v1.y * v2.x));

    return atan2(cross_mag, dot);
}

} // namespace physics::eom
