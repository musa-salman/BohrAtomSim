#include <cmath>

#include "eom/spherical.hpp"
#include "eom/utils.hpp"

namespace eom {

namespace spherical {

scalar compute_r_0(const eom::Vector3 &r0) noexcept { return r0.magnitude(); }

scalar compute_r_dot_0(const eom::Vector3 &r0,
                       const eom::Vector3 &v0) noexcept {
    const scalar r_dot = r0.dot(v0) / r0.magnitude();
    return r_dot;
}

scalar compute_theta_0(const eom::Vector3 &r0) noexcept {
    return acos(r0.z / r0.magnitude());
}

scalar compute_theta_0(quantum_magnetic magnetic,
                       quantum_angular angular) noexcept {
    return std::asin(static_cast<scalar>(magnetic) / angular);
}

scalar compute_theta_dot_0(const eom::Vector3 &r0,
                           const eom::Vector3 &v0) noexcept {
    const scalar r = r0.magnitude();
    const scalar rho = sqrt(r0.x * r0.x + r0.y * r0.y);

    if (r == 0 || rho == 0)
        return 0;

    eom::Vector3 theta_hat = {(r0.z * r0.x) / (r * rho),
                              (r0.z * r0.y) / (r * rho), -rho / r};

    return v0.dot(theta_hat) / r;
}

scalar compute_phi_0(const eom::Vector3 &r0) noexcept {
    return atan2(r0.y, r0.x);
}

scalar compute_phi_dot_0(quantum_angular angular, quantum_magnetic magnetic,
                         scalar radius) noexcept {
    return SQUARE(angular) / (SQUARE(radius) * magnetic);
}

scalar compute_p_phi(const eom::Vector3 &r0, const eom::Vector3 &v0) noexcept {
    const scalar angle = r0.angle_with(v0);
    return r0.magnitude() * v0.magnitude() * sin(angle);
}

} // namespace spherical
} // namespace eom