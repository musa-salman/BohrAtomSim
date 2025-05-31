#ifndef POLAR_REL_EOM_HPP
#define POLAR_REL_EOM_HPP

#include "eom/utils.hpp"
#include "utils/types.h"

namespace eom {

namespace polar {

namespace rel {

SIM_CONST SIM_INLINE inline scalar compute_gamma(scalar radius, scalar r_dot,
                                                 scalar p_psi) {
    const scalar term1 =
        SQUARE(p_psi) * INV_SPEED_OF_LIGHT_SQUARED / SQUARE(radius);
    const scalar term2 = SQUARE(r_dot) * INV_SPEED_OF_LIGHT_SQUARED;
    const scalar result = sqrt((1 + term1) / (1 - term2));
    return result;
}

SIM_CONST SIM_INLINE inline scalar compute_psi_dot(scalar radius, scalar gamma,
                                                   scalar p_psi) noexcept {
    const scalar result = p_psi / (SQUARE(radius) * gamma);
    return result;
}

SIM_CONST SIM_INLINE inline scalar compute_r_ddot(scalar r, scalar r_dot,
                                                  scalar gamma, scalar p_psi,
                                                  scalar du_dr) noexcept {
    const scalar term1 = SQUARE(p_psi) / (SQUARE(gamma) * SQUARE(r) * r);
    const scalar term2 = du_dr / gamma;
    const scalar term3 = SQUARE(r_dot) * INV_SPEED_OF_LIGHT_SQUARED;

    const scalar result = term1 - term2 * (1 - term3);
    return result;
}

struct EOM {
    template <typename State, typename PotEval>
    static SIM_INLINE inline void update(State &s,
                                         const PotEval &m_du) noexcept {
        s.gamma = compute_gamma(s.r, s.r_dot, s.p_psi);
        s.r_ddot = compute_r_ddot(s.r, s.r_dot, s.gamma, s.p_psi, m_du.dU_dr());
        s.psi_dot = compute_psi_dot(s.r, s.gamma, s.p_psi);
    }
};

} // namespace rel
} // namespace polar
} // namespace eom

#endif // POLAR_REL_EOM_HPP