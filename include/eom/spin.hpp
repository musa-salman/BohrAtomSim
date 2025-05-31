#ifndef SPIN_EOM_HPP
#define SPIN_EOM_HPP

#include <math.h>
#include <sleef.h>

#include "utils.hpp"
#include "utils/types.h"

namespace eom {
namespace spin {

SIM_CONST SIM_INLINE inline scalar compute_gamma(scalar radius, scalar r_dot,
                                                 scalar theta, scalar theta_dot,
                                                 quantum_angular angular) {
    const scalar sin_theta = sin(theta);

    const scalar term1 = 4 * SQUARE(SPEED_OF_LIGHT_SQUARE * radius);
    const scalar term2 =
        SPEED_OF_LIGHT_SQUARE * SQUARE(sin_theta * radius) + SQUARE(angular);
    const scalar term3 = term1 * term2;

    const scalar term4 = 4 * SPEED_OF_LIGHT_SQUARE * G_FACTOR * S_Z_FACTOR *
                         radius * SQUARE(sin_theta) * angular;
    const scalar term5 =
        SQUARE(G_FACTOR * S_Z_FACTOR) * SQUARE(SQUARE(sin_theta));
    const scalar term6 = -term3 + term4 - term5;

    const scalar term7 =
        SQUARE(theta_dot * radius) + SQUARE(r_dot) - SPEED_OF_LIGHT_SQUARE;
    const scalar term8 = term6 / term7;

    const scalar term9 = 2 * SPEED_OF_LIGHT_SQUARE * SQUARE(radius) * sin_theta;

    const scalar result = sqrt(term8) / term9;

    return result;
}

SIM_CONST SIM_INLINE inline scalar compute_phi_dot_0(quantum_angular angular,
                                                     quantum_magnetic magnetic,
                                                     scalar radius,
                                                     scalar gamma) noexcept {
    const scalar term1 = SQUARE(angular) / (gamma * magnetic * SQUARE(radius));

    const scalar term2 = 0.5 * G_FACTOR * S_Z_FACTOR *
                         INV_SPEED_OF_LIGHT_SQUARED /
                         (gamma * SQUARE(radius) * radius);

    const scalar result = term1 - term2;

    return result;
}

SIM_CONST SIM_INLINE inline scalar compute_phi_dot(scalar radius, scalar theta,
                                                   scalar gamma,
                                                   quantum_magnetic magnetic) {
    const scalar sin_theta = sin(theta);

    const scalar term1 =
        magnetic / (gamma * SQUARE(radius) * SQUARE(sin_theta));

    const scalar term2 = 0.5 * G_FACTOR * S_Z_FACTOR *
                         INV_SPEED_OF_LIGHT_SQUARED /
                         (gamma * SQUARE(radius) * radius);

    const scalar result = term1 - term2;

    return result;
}

SIM_CONST SIM_INLINE inline scalar
compute_theta_ddot(scalar phi_dot, scalar radius, scalar r_dot, scalar theta,
                   scalar theta_dot, scalar gamma) noexcept {
    const scalar sin_cos_theta = sin(theta) * cos(theta);

    const scalar term1 = SQUARE(phi_dot) * sin_cos_theta;

    const scalar term2 = 2 * theta_dot * r_dot / radius;
    const scalar term3 = 0.5 * INV_SPEED_OF_LIGHT_SQUARED / (gamma * radius);

    const scalar term4 = G_FACTOR * S_Z_FACTOR *
                         SQUARE(INV_SPEED_OF_LIGHT_SQUARED) * phi_dot *
                         SQUARE(theta_dot) * sin_cos_theta / (gamma * radius);

    const scalar result = term1 - term2 * (1 - term3) + term4;

    return result;
}

SIM_CONST SIM_INLINE inline scalar compute_r_ddot(scalar phi_dot, scalar radius,
                                                  scalar r_dot, scalar theta,
                                                  scalar theta_dot,
                                                  scalar gamma) {
    const scalar sin_theta = sin(theta);

    const scalar term1 = SQUARE(phi_dot * sin_theta) * radius;

    const scalar term2 = SQUARE(theta_dot) * radius;

    const scalar term3 = 1 / (gamma * SQUARE(radius));
    const scalar term4 = SQUARE(r_dot) * INV_SPEED_OF_LIGHT_SQUARED;

    const scalar term5 = 0.5 * G_FACTOR * S_Z_FACTOR *
                         INV_SPEED_OF_LIGHT_SQUARED * phi_dot *
                         SQUARE(sin_theta) / (gamma * SQUARE(radius));
    const scalar term6 = 2 * theta_dot * r_dot * radius *
                         INV_SPEED_OF_LIGHT_SQUARED * cos(theta) / sin_theta;

    const scalar result =
        term1 + term2 - term3 * (1 - term4) - term5 * (1 - term6);

    return result;
}

} // namespace spin
} // namespace eom

#endif // SPHERICAL_REL_EOM_HPP