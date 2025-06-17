
#include <cmath>
#include <sys/types.h>

#include "physics/eom/polar.hpp"
#include "physics/math/Vector2.hpp"
#include "physics/math/VectorOps.hpp"
#include "physics/math/constants.hpp"
#include "physics/math/types.hpp"

namespace physics::eom::polar {

math::scalar compute_r_0(const math::Vector2 &r0) noexcept {
    return math::magnitude(r0);
}

math::scalar compute_r_dot_0(const math::Vector2 &r0,
                             const math::Vector2 &v0) noexcept {
    const math::scalar r_dot = math::dot(r0, v0) / math::magnitude(r0);
    return r_dot;
}

math::scalar compute_psi_0(const math::Vector2 &r0) noexcept {
    const math::scalar result = atan2(r0.y, r0.x);

    // Normalize the angle to the range [0, 2pi)
    uint npi = static_cast<uint>(result / math::TWO_PI);
    return result - math::TWO_PI * static_cast<math::scalar>(npi);
}

math::scalar compute_p_psi(const math::Vector2 &r0,
                           const math::Vector2 &v0) noexcept {
    const math::scalar angle = math::angle_with(r0, v0);
    return math::magnitude(r0) * math::magnitude(v0) * math::sin(angle);
}

} // namespace physics::eom::polar