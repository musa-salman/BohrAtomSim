
#include "eom/utils.hpp"

namespace eom {

eom::radial_bounds compute_radial_limits(quantum_principle principle,
                                         quantum_angular angular) {
    const scalar term1 = SQUARE(principle);
    const scalar term2 = SQUARE(angular) / term1;

    const scalar term3 = sqrt(1 - term2);

    eom::radial_bounds radial_bounds;

    radial_bounds.r_min = term1 * (1 - term3);
    radial_bounds.r_max = term1 * (1 + term3);

    return radial_bounds;
}

eom::Vector3 spherical_to_cartesian(scalar radius, scalar theta, scalar phi) {
    eom::Vector3 vec;

    vec.x = radius * eom::sin(theta) * eom::cos(phi);
    vec.y = radius * eom::sin(theta) * eom::sin(phi);
    vec.z = radius * eom::cos(theta);

    return vec;
}

scalar compute_angular_distance(const eom::Vector3 &v1,
                                const eom::Vector3 &v2) {
    const scalar dot = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    const scalar cross_mag = eom::sqrt(SQUARE(v1.y * v2.z - v1.z * v2.y) +
                                       SQUARE(v1.z * v2.x - v1.x * v2.z) +
                                       SQUARE(v1.x * v2.y - v1.y * v2.x));

    return atan2(cross_mag, dot);
}

} // namespace eom