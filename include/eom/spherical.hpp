#ifndef SPHERICAL_EOM_H
#define SPHERICAL_EOM_H

#include <math.h>
#include <sleef.h>

#include "utils.hpp"
#include "utils/types.h"

namespace eom {

namespace spherical {

SIM_CONST scalar compute_r_0(const eom::Vector3 &r) noexcept;

SIM_CONST scalar compute_r_dot_0(const eom::Vector3 &r,
                                 const eom::Vector3 &v) noexcept;

SIM_CONST scalar compute_theta_0(const eom::Vector3 &r) noexcept;

SIM_CONST scalar compute_theta_0(quantum_magnetic magnetic,
                                 quantum_angular angular) noexcept;

SIM_CONST scalar compute_theta_dot_0(const eom::Vector3 &r,
                                     const eom::Vector3 &v) noexcept;

SIM_CONST scalar compute_phi_0(const eom::Vector3 &r) noexcept;

SIM_CONST scalar compute_p_phi(const eom::Vector3 &r,
                               const eom::Vector3 &v) noexcept;

namespace non_rel {

struct EOM {
    template <typename State, typename Potential>
    static SIM_INLINE void update(State &s, Potential &m_du) noexcept {
        s.r_ddot =
            compute_r_ddot(s.r, s.theta, s.theta_dot, s.phi_dot, m_du.dU_dr());
        s.theta_ddot =
            compute_theta_ddot(s.r, s.r_dot, s.theta, s.theta_dot, s.phi_dot);
        s.phi_dot = compute_phi_dot(s.r, s.theta, s.p_phi);
    }
};

SIM_CONST SIM_INLINE inline scalar compute_phi_dot(scalar radius, scalar theta,
                                                   scalar p_phi) noexcept {
    const scalar sin_theta = sin(theta);
    const scalar result = p_phi / (SQUARE(radius * sin_theta));

    return result;
}

SIM_CONST SIM_INLINE inline scalar compute_theta_ddot(scalar r, scalar r_dot,
                                                      scalar theta,
                                                      scalar theta_dot,
                                                      scalar phi_dot) noexcept {
    const scalar term1 = 0.5 * sin(2 * theta) * SQUARE(phi_dot);
    const scalar term2 = 2 * r_dot * theta_dot / r;

    const scalar result = term1 - term2;

    return result;
}

SIM_CONST SIM_INLINE inline scalar compute_r_ddot(scalar radius, scalar theta,
                                                  scalar theta_dot,
                                                  scalar phi_dot,
                                                  scalar du) noexcept {
    const scalar sin_theta = sin(theta);

    const scalar term1 = SQUARE(phi_dot * sin_theta) * radius;
    const scalar term2 = SQUARE(theta_dot) * radius;
    const scalar term3 = -du;

    const scalar result = term1 + term2 + term3;
    return result;
}

} // namespace non_rel
} // namespace spherical
} // namespace eom

#endif // SPHERICAL_EOM_H