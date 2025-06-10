#pragma once

#include "physics/math/Vector3.hpp"
#include "physics/math/macros.hpp"
#include "physics/math/types.hpp"

namespace physics::eom {

math::Vector3 sphericalToCartesian(math::scalar radius, math::scalar theta,
                                   math::scalar phi);

math::scalar computeAngularDistance(const math::Vector3 &v1,
                                    const math::Vector3 &v2);

SIM_CONST SIM_INLINE bool isLocalMax(math::scalar prevVarDot,
                                     math::scalar currVarDot) noexcept {
    return prevVarDot < 0 && currVarDot > 0;
}

#define SQUARE(x) ((x) * (x))

} // namespace physics::eom
