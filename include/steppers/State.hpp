#ifndef STATE_HPP
#define STATE_HPP

#include <array>

#include "eom/utils.hpp"

struct Relativistic {
    // scalar gamma;
};

struct Quantized {
    Quantized() : delta_psi(0) {}

    scalar delta_psi;
};

struct State {
    State() : time(0) {}

    template <typename... Fields>
    SIM_CONST SIM_INLINE inline std::array<scalar, sizeof...(Fields)>
    export_fields(const Fields &...fields) const noexcept {
        return {static_cast<scalar>(fields)...};
    }

    scalar time;
};

#endif // STATE_HPP