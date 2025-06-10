#ifndef POLAR_REL_EOM_HPP
#define POLAR_REL_EOM_HPP

#include "physics/math/constants.hpp"
#include "physics/math/intrinsics.hpp"
#include "physics/math/macros.hpp"
#include "physics/math/types.hpp"

namespace physics::eom {

namespace polar {

namespace rel {

SIM_CONST SIM_INLINE math::scalar
compute_gamma(math::scalar radius, math::scalar r_dot, math::scalar p_psi) {
    const math::scalar term1 =
        SQUARE(p_psi) * math::INV_SPEED_OF_LIGHT_SQUARED / SQUARE(radius);
    const math::scalar term2 = SQUARE(r_dot) * math::INV_SPEED_OF_LIGHT_SQUARED;
    const math::scalar result = math::sqrt((1 + term1) / (1 - term2));
    return result;
}

SIM_CONST SIM_INLINE math::scalar compute_psi_dot(math::scalar radius,
                                                  math::scalar gamma,
                                                  math::scalar p_psi) noexcept {
    const math::scalar result = p_psi / (SQUARE(radius) * gamma);
    return result;
}

SIM_CONST SIM_INLINE math::scalar
compute_r_ddot(math::scalar r, math::scalar r_dot, math::scalar gamma,
               math::scalar p_psi, math::scalar du_dr) noexcept {
    const math::scalar term1 = SQUARE(p_psi) / (SQUARE(gamma) * SQUARE(r) * r);
    const math::scalar term2 = du_dr / gamma;
    const math::scalar term3 = SQUARE(r_dot) * math::INV_SPEED_OF_LIGHT_SQUARED;

    const math::scalar result = term1 - term2 * (1 - term3);
    return result;
}

struct EOM {
    template <typename State, typename PotEval>
    static SIM_INLINE void update(State &s, const PotEval &m_du) noexcept {
        s.gamma = compute_gamma(s.r, s.r_dot, s.p_psi);
        s.r_ddot = compute_r_ddot(s.r, s.r_dot, s.gamma, s.p_psi, m_du.dU_dr());
        s.psi_dot = compute_psi_dot(s.r, s.gamma, s.p_psi);
    }
};

} // namespace rel
} // namespace polar
} // namespace physics::eom

#endif // POLAR_REL_EOM_HPP