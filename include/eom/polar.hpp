#ifndef POLAR_EOM_HPP
#define POLAR_EOM_HPP

#include "eom/utils.hpp"
#include "utils/types.h"

namespace eom {

namespace polar {

SIM_CONST scalar compute_r_0(const Vector2 &r0) noexcept;

SIM_CONST scalar compute_r_dot_0(const Vector2 &r0, const Vector2 &v0) noexcept;

SIM_CONST scalar compute_psi_0(const Vector2 &r0) noexcept;

SIM_CONST scalar compute_p_psi(const Vector2 &r0, const Vector2 &v0) noexcept;

namespace non_rel {

SIM_CONST SIM_INLINE inline scalar compute_psi_dot(scalar radius,
                                                   scalar p_psi) noexcept {
    const scalar result = p_psi / SQUARE(radius);
    return result;
}

SIM_CONST SIM_INLINE inline scalar compute_r_ddot(scalar radius, scalar p_psi,
                                                  scalar du) noexcept {
    const scalar result = SQUARE(p_psi) / (radius * SQUARE(radius)) - du;
    return result;
}

struct EOM {
    template <typename State, typename PotEval>
    static SIM_INLINE void update(State &s, const PotEval &m_du) noexcept {
        s.r_ddot = compute_r_ddot(s.r, s.p_psi, m_du.dU_dr());
        s.psi_dot = compute_psi_dot(s.r, s.p_psi);
    }
};

} // namespace non_rel
} // namespace polar
} // namespace eom

#endif // POLAR_EOM_HPP