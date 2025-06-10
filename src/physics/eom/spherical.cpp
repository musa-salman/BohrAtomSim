#include <cmath>

#include "physics/eom/spherical.hpp"
#include "physics/math/Vector3.hpp"
#include "physics/math/VectorOps.hpp"
#include "physics/math/types.hpp"

namespace physics::eom::spherical {

math::scalar compute_r_0(const math::Vector3 &r0) noexcept {
    return math::magnitude(r0);
}

math::scalar compute_r_dot_0(const math::Vector3 &r0,
                             const math::Vector3 &v0) noexcept {
    const math::scalar r_dot = math::dot(r0, v0) / math::magnitude(r0);
    return r_dot;
}

math::scalar compute_theta_0(const math::Vector3 &r0) noexcept {
    return acos(r0.z / math::magnitude(r0));
}

math::scalar compute_theta_dot_0(const math::Vector3 &r0,
                                 const math::Vector3 &v0) noexcept {
    const math::scalar r = math::magnitude(r0);
    const math::scalar rho = sqrt(r0.x * r0.x + r0.y * r0.y);

    if (r == 0 || rho == 0)
        return 0;

    math::Vector3 theta_hat = {(r0.z * r0.x) / (r * rho),
                               (r0.z * r0.y) / (r * rho), -rho / r};

    return math::dot(v0, theta_hat) / r;
}

math::scalar compute_phi_0(const math::Vector3 &r0) noexcept {
    return atan2(r0.y, r0.x);
}

math::scalar compute_phi_dot_0(math::quantum_angular angular,
                               math::quantum_magnetic magnetic,
                               math::scalar radius) noexcept {
    return SQUARE(angular) / (SQUARE(radius) * magnetic);
}

math::scalar compute_p_phi(const math::Vector3 &r0,
                           const math::Vector3 &v0) noexcept {
    const math::scalar angle = math::angle_with(r0, v0);
    return math::magnitude(r0) * math::magnitude(v0) * std::sin(angle);
}

} // namespace physics::eom::spherical