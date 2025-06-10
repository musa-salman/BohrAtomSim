#pragma once

#include "physics/math/macros.hpp"
#include "physics/math/types.hpp"

namespace physics::potential {

struct PotentialEvaluator {
    SIM_CONST SIM_INLINE math::scalar dU_dr(this const auto &self) noexcept {
        return self.dU_dr();
    }
};
} // namespace physics::potential
