#pragma once

#include "PotentialEvaluator.hpp"
#include "physics/math/macros.hpp"
#include "physics/math/types.hpp"

namespace physics::potential {
struct CoulombPotential : PotentialEvaluator {
    CoulombPotential(math::scalar &r) : m_r(r) {}

    CoulombPotential &operator=(const CoulombPotential &other) {
        if (this != &other)
            m_r = other.m_r;
        return *this;
    }

    CoulombPotential(const CoulombPotential &other) : m_r(other.m_r) {}

    CoulombPotential rebind(math::scalar &new_r) const {
        return CoulombPotential(new_r);
    }

    SIM_CONST SIM_INLINE math::scalar dU_dr() const {
        const math::scalar result = 1.0 / (m_r * m_r);
        return result;
    }

  private:
    math::scalar &m_r;
};
} // namespace physics::potential
