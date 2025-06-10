#ifndef SPHERICAL_EOM_H
#define SPHERICAL_EOM_H

#include <math.h>
#include <sleef.h>

#include "physics/eom/eom_common.hpp"
#include "physics/math/macros.hpp"
#include "physics/math/types.hpp"

namespace physics::eom {

namespace spherical {

SIM_CONST math::scalar compute_r_0(const math::Vector3 &r) noexcept;

SIM_CONST math::scalar compute_r_dot_0(const math::Vector3 &r,
                                       const math::Vector3 &v) noexcept;

SIM_CONST math::scalar compute_theta_0(const math::Vector3 &r) noexcept;

SIM_CONST math::scalar compute_theta_dot_0(const math::Vector3 &r,
                                           const math::Vector3 &v) noexcept;

SIM_CONST math::scalar compute_phi_0(const math::Vector3 &r) noexcept;

SIM_CONST math::scalar compute_p_phi(const math::Vector3 &r,
                                     const math::Vector3 &v) noexcept;

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

SIM_CONST SIM_INLINE math::scalar compute_phi_dot(math::scalar radius,
                                                  math::scalar theta,
                                                  math::scalar p_phi) noexcept {
    const math::scalar sin_theta = sin(theta);
    const math::scalar result = p_phi / (SQUARE(radius * sin_theta));

    return result;
}

SIM_CONST SIM_INLINE math::scalar
compute_theta_ddot(math::scalar r, math::scalar r_dot, math::scalar theta,
                   math::scalar theta_dot, math::scalar phi_dot) noexcept {
    const math::scalar term1 = 0.5 * sin(2 * theta) * SQUARE(phi_dot);
    const math::scalar term2 = 2 * r_dot * theta_dot / r;

    const math::scalar result = term1 - term2;

    return result;
}

SIM_CONST SIM_INLINE math::scalar
compute_r_ddot(math::scalar radius, math::scalar theta, math::scalar theta_dot,
               math::scalar phi_dot, math::scalar du) noexcept {
    const math::scalar sin_theta = sin(theta);

    const math::scalar term1 = SQUARE(phi_dot * sin_theta) * radius;
    const math::scalar term2 = SQUARE(theta_dot) * radius;
    const math::scalar term3 = -du;

    const math::scalar result = term1 + term2 + term3;
    return result;
}

} // namespace non_rel
} // namespace spherical
} // namespace physics::eom

#endif // SPHERICAL_EOM_H