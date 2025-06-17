#ifndef SPIN_EOM_HPP
#define SPIN_EOM_HPP

#include <math.h>
#include <sleef.h>

#include "physics/math/constants.hpp"
#include "physics/math/macros.hpp"
#include "physics/math/types.hpp"

namespace eom {
namespace spin {

SIM_CONST SIM_INLINE physics::math::scalar
compute_gamma(physics::math::scalar radius, physics::math::scalar r_dot,
              physics::math::scalar theta, physics::math::scalar theta_dot,
              physics::math::quantum_angular angular) {
    const physics::math::scalar sin_theta = sin(theta);

    const physics::math::scalar term1 =
        4 * SQUARE(physics::math::SPEED_OF_LIGHT_SQUARE * radius);
    const physics::math::scalar term2 =
        physics::math::SPEED_OF_LIGHT_SQUARE * SQUARE(sin_theta * radius) +
        SQUARE(angular);
    const physics::math::scalar term3 = term1 * term2;

    const physics::math::scalar term4 =
        4 * physics::math::SPEED_OF_LIGHT_SQUARE * physics::math::G_FACTOR *
        physics::math::S_Z_FACTOR * radius * SQUARE(sin_theta) * angular;
    const physics::math::scalar term5 =
        SQUARE(physics::math::G_FACTOR * physics::math::S_Z_FACTOR) *
        SQUARE(SQUARE(sin_theta));
    const physics::math::scalar term6 = -term3 + term4 - term5;

    const physics::math::scalar term7 = SQUARE(theta_dot * radius) +
                                        SQUARE(r_dot) -
                                        physics::math::SPEED_OF_LIGHT_SQUARE;
    const physics::math::scalar term8 = term6 / term7;

    const physics::math::scalar term9 =
        2 * physics::math::SPEED_OF_LIGHT_SQUARE * SQUARE(radius) * sin_theta;

    const physics::math::scalar result = sqrt(term8) / term9;

    return result;
}

SIM_CONST SIM_INLINE physics::math::scalar
compute_phi_dot_0(physics::math::quantum_angular angular,
                  physics::math::quantum_magnetic magnetic,
                  physics::math::scalar radius,
                  physics::math::scalar gamma) noexcept {
    const physics::math::scalar term1 =
        SQUARE(angular) / (gamma * magnetic * SQUARE(radius));

    const physics::math::scalar term2 = 0.5 * physics::math::G_FACTOR *
                                        physics::math::S_Z_FACTOR *
                                        physics::math::SPEED_OF_LIGHT_SQUARE /
                                        (gamma * SQUARE(radius) * radius);

    const physics::math::scalar result = term1 - term2;

    return result;
}

SIM_CONST SIM_INLINE physics::math::scalar
compute_phi_dot(physics::math::scalar radius, physics::math::scalar theta,
                physics::math::scalar gamma,
                physics::math::quantum_magnetic magnetic) {
    const physics::math::scalar sin_theta = sin(theta);

    const physics::math::scalar term1 =
        magnetic / (gamma * SQUARE(radius) * SQUARE(sin_theta));

    const physics::math::scalar term2 = 0.5 * physics::math::G_FACTOR *
                                        physics::math::S_Z_FACTOR *
                                        physics::math::SPEED_OF_LIGHT_SQUARE /
                                        (gamma * SQUARE(radius) * radius);

    const physics::math::scalar result = term1 - term2;

    return result;
}

SIM_CONST SIM_INLINE physics::math::scalar
compute_theta_ddot(physics::math::scalar phi_dot, physics::math::scalar radius,
                   physics::math::scalar r_dot, physics::math::scalar theta,
                   physics::math::scalar theta_dot,
                   physics::math::scalar gamma) noexcept {
    const physics::math::scalar sin_cos_theta = sin(theta) * cos(theta);

    const physics::math::scalar term1 = SQUARE(phi_dot) * sin_cos_theta;

    const physics::math::scalar term2 = 2 * theta_dot * r_dot / radius;
    const physics::math::scalar term3 =
        0.5 * physics::math::SPEED_OF_LIGHT_SQUARE / (gamma * radius);

    const physics::math::scalar term4 =
        physics::math::G_FACTOR * physics::math::S_Z_FACTOR *
        SQUARE(physics::math::SPEED_OF_LIGHT_SQUARE) * phi_dot *
        SQUARE(theta_dot) * sin_cos_theta / (gamma * radius);

    const physics::math::scalar result = term1 - term2 * (1 - term3) + term4;

    return result;
}

SIM_CONST SIM_INLINE physics::math::scalar
compute_r_ddot(physics::math::scalar phi_dot, physics::math::scalar radius,
               physics::math::scalar r_dot, physics::math::scalar theta,
               physics::math::scalar theta_dot, physics::math::scalar gamma) {
    const physics::math::scalar sin_theta = sin(theta);

    const physics::math::scalar term1 = SQUARE(phi_dot * sin_theta) * radius;

    const physics::math::scalar term2 = SQUARE(theta_dot) * radius;

    const physics::math::scalar term3 = 1 / (gamma * SQUARE(radius));
    const physics::math::scalar term4 =
        SQUARE(r_dot) * physics::math::SPEED_OF_LIGHT_SQUARE;

    const physics::math::scalar term5 =
        0.5 * physics::math::G_FACTOR * physics::math::S_Z_FACTOR *
        physics::math::SPEED_OF_LIGHT_SQUARE * phi_dot * SQUARE(sin_theta) /
        (gamma * SQUARE(radius));
    const physics::math::scalar term6 = 2 * theta_dot * r_dot * radius *
                                        physics::math::SPEED_OF_LIGHT_SQUARE *
                                        cos(theta) / sin_theta;

    const physics::math::scalar result =
        term1 + term2 - term3 * (1 - term4) - term5 * (1 - term6);

    return result;
}

} // namespace spin
} // namespace eom

#endif // SPHERICAL_REL_EOM_HPP