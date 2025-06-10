#pragma once

#include "macros.hpp"
#include "types.hpp"

namespace physics::math {

struct RadialBounds {
    scalar r_min;
    scalar r_max;
};

RadialBounds computeRadialLimits(quantum_principle principal,
                                 quantum_angular angular);

SIM_CONST scalar computeInitialTheta(quantum_magnetic magnetic,
                                     quantum_angular angular) noexcept;

} // namespace physics::math