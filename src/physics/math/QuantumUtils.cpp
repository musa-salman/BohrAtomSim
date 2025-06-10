

#include "physics/math/QuantumUtils.hpp"
#include "physics/math/intrinsics.hpp"
#include "physics/math/macros.hpp"

namespace physics::math {

RadialBounds computeRadialLimits(quantum_principle principle,
                                 quantum_angular angular) {
    const scalar term1 = SQUARE(principle);
    const scalar term2 = SQUARE(angular) / term1;

    const scalar term3 = sqrt(1 - term2);

    RadialBounds radialBounds;

    radialBounds.r_min = term1 * (1 - term3);
    radialBounds.r_max = term1 * (1 + term3);

    return radialBounds;
}

scalar computeInitialTheta(quantum_magnetic magnetic,
                           quantum_angular angular) noexcept {
    return std::asin(static_cast<scalar>(magnetic) / angular);
}

} // namespace physics::math