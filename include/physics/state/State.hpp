#pragma once

#include <array>

#include "physics/math/macros.hpp"
#include "physics/math/types.hpp"

namespace physics::state {

struct Relativistic {
    // math::scalar gamma;
};

struct Quantized {
    Quantized() : delta_psi(0) {}

    math::scalar delta_psi;
};

struct State {
    State() : time(0) {}

    template <typename... Fields>
    SIM_CONST SIM_INLINE std::array<math::scalar, sizeof...(Fields)>
    export_fields(const Fields &...fields) const noexcept {
        return {static_cast<math::scalar>(fields)...};
    }

    math::scalar time;
};

} // namespace physics::state