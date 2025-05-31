#include "eom/polar.hpp"
#include "eom/utils.hpp"

namespace eom {

namespace polar {

scalar compute_r_0(const eom::Vector2 &r0) noexcept { return r0.magnitude(); }

scalar compute_r_dot_0(const eom::Vector2 &r0,
                       const eom::Vector2 &v0) noexcept {
    const scalar r_dot = r0.dot(v0) / r0.magnitude();
    return r_dot;
}

scalar compute_psi_0(const eom::Vector2 &r0) noexcept {
    const scalar result = atan2(r0.y, r0.x);

    // Normalize the angle to the range [0, 2pi)
    uint npi = static_cast<uint>(result / TWO_PI);
    return result - TWO_PI * static_cast<scalar>(npi);
}

scalar compute_p_psi(const eom::Vector2 &r0, const eom::Vector2 &v0) noexcept {
    const scalar angle = r0.angle_with(v0);
    return r0.magnitude() * v0.magnitude() * eom::sin(angle);
}

} // namespace polar
} // namespace eom