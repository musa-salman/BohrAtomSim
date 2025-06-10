#ifndef POLAR_EOM_HPP
#define POLAR_EOM_HPP

#include "physics/math/Vector2.hpp"
#include "physics/math/macros.hpp"
#include "physics/math/types.hpp"

namespace physics::eom {

namespace polar {

SIM_CONST math::scalar compute_r_0(const math::Vector2 &r0) noexcept;

SIM_CONST math::scalar compute_r_dot_0(const math::Vector2 &r0,
                                       const math::Vector2 &v0) noexcept;

SIM_CONST math::scalar compute_psi_0(const math::Vector2 &r0) noexcept;

SIM_CONST math::scalar compute_p_psi(const math::Vector2 &r0,
                                     const math::Vector2 &v0) noexcept;

namespace non_rel {

SIM_CONST SIM_INLINE math::scalar compute_psi_dot(math::scalar radius,
                                                  math::scalar p_psi) noexcept {
    const math::scalar result = p_psi / SQUARE(radius);
    return result;
}

SIM_CONST SIM_INLINE math::scalar compute_r_ddot(math::scalar radius,
                                                 math::scalar p_psi,
                                                 math::scalar du) noexcept {
    const math::scalar result = SQUARE(p_psi) / (radius * SQUARE(radius)) - du;
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
} // namespace physics::eom

#endif // POLAR_EOM_HPP