#ifndef SPHERICAL_REL_EOM_HPP
#define SPHERICAL_REL_EOM_HPP

#include <math.h>
#include <sleef.h>

#include "utils.hpp"
#include "utils/types.h"

namespace eom {

namespace spherical {

namespace rel {

SIM_CONST SIM_INLINE inline scalar compute_gamma(scalar radius, scalar r_dot,
                                                 scalar theta, scalar theta_dot,
                                                 scalar p_phi) noexcept {
    const scalar sin_theta = sin(theta);

    const scalar term1 = SQUARE(p_phi) * INV_SPEED_OF_LIGHT_SQUARED /
                         (SQUARE(sin_theta) * SQUARE(radius));
    const scalar term2 = (SQUARE(r_dot) + SQUARE(theta_dot) * SQUARE(radius)) *
                         INV_SPEED_OF_LIGHT_SQUARED;

    const scalar result = sqrt((1 + term1) / (1 - term2));
    return result;
}

SIM_CONST SIM_INLINE inline scalar compute_phi_dot(scalar radius, scalar theta,
                                                   scalar gamma,
                                                   scalar p_phi) noexcept {
    const scalar sin_theta = sin(theta);
    const scalar result = p_phi / (SQUARE(radius * sin_theta) * gamma);

    return result;
}

SIM_CONST SIM_INLINE inline scalar
compute_theta_ddot(scalar r, scalar r_dot, scalar theta, scalar theta_dot,
                   scalar phi_dot, scalar gamma, scalar du) noexcept {

    const scalar term1 = 0.5 * sin(2 * theta) * SQUARE(phi_dot);
    const scalar term2 = 2 * r_dot * theta_dot / r;
    const scalar term3 = 0.5 * INV_SPEED_OF_LIGHT_SQUARED * du * r / gamma;

    const scalar result = term1 - term2 * (1 - term3);

    return result;
}

SIM_CONST SIM_INLINE inline scalar
compute_r_ddot(scalar radius, scalar r_dot, scalar theta, scalar theta_dot,
               scalar phi_dot, scalar gamma, scalar du) noexcept {
    const scalar sin_theta = sin(theta);

    const scalar term1 = SQUARE(phi_dot * sin_theta) * radius;
    const scalar term2 = SQUARE(theta_dot) * radius;
    const scalar term3 = -du / gamma;
    const scalar term4 = SQUARE(r_dot) * INV_SPEED_OF_LIGHT_SQUARED;

    const scalar result = term1 + term2 + term3 * (1 - term4);
    return result;
}

} // namespace rel
} // namespace spherical
} // namespace eom

#endif // SPHERICAL_REL_EOM_HPP