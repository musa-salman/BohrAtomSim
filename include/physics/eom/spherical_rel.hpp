#ifndef SPHERICAL_REL_EOM_HPP
#define SPHERICAL_REL_EOM_HPP

#include <math.h>
#include <sleef.h>

#include "physics/math/constants.hpp"
#include "physics/math/macros.hpp"
#include "physics/math/types.hpp"

namespace physics::eom {

namespace spherical {

namespace rel {

SIM_CONST SIM_INLINE math::scalar
compute_gamma(math::scalar radius, math::scalar r_dot, math::scalar theta,
              math::scalar theta_dot, math::scalar p_phi) noexcept {
    const math::scalar sin_theta = sin(theta);

    const math::scalar term1 = SQUARE(p_phi) *
                               math::INV_SPEED_OF_LIGHT_SQUARED /
                               (SQUARE(sin_theta) * SQUARE(radius));
    const math::scalar term2 =
        (SQUARE(r_dot) + SQUARE(theta_dot) * SQUARE(radius)) *
        math::INV_SPEED_OF_LIGHT_SQUARED;

    const math::scalar result = sqrt((1 + term1) / (1 - term2));
    return result;
}

SIM_CONST SIM_INLINE math::scalar compute_phi_dot(math::scalar radius,
                                                  math::scalar theta,
                                                  math::scalar gamma,
                                                  math::scalar p_phi) noexcept {
    const math::scalar sin_theta = sin(theta);
    const math::scalar result = p_phi / (SQUARE(radius * sin_theta) * gamma);

    return result;
}

SIM_CONST SIM_INLINE math::scalar
compute_theta_ddot(math::scalar r, math::scalar r_dot, math::scalar theta,
                   math::scalar theta_dot, math::scalar phi_dot,
                   math::scalar gamma, math::scalar du) noexcept {

    const math::scalar term1 = 0.5 * sin(2 * theta) * SQUARE(phi_dot);
    const math::scalar term2 = 2 * r_dot * theta_dot / r;
    const math::scalar term3 =
        0.5 * math::INV_SPEED_OF_LIGHT_SQUARED * du * r / gamma;

    const math::scalar result = term1 - term2 * (1 - term3);

    return result;
}

SIM_CONST SIM_INLINE math::scalar
compute_r_ddot(math::scalar radius, math::scalar r_dot, math::scalar theta,
               math::scalar theta_dot, math::scalar phi_dot, math::scalar gamma,
               math::scalar du) noexcept {
    const math::scalar sin_theta = sin(theta);

    const math::scalar term1 = SQUARE(phi_dot * sin_theta) * radius;
    const math::scalar term2 = SQUARE(theta_dot) * radius;
    const math::scalar term3 = -du / gamma;
    const math::scalar term4 = SQUARE(r_dot) * math::INV_SPEED_OF_LIGHT_SQUARED;

    const math::scalar result = term1 + term2 + term3 * (1 - term4);
    return result;
}

} // namespace rel
} // namespace spherical
} // namespace physics::eom

#endif // SPHERICAL_REL_EOM_HPP