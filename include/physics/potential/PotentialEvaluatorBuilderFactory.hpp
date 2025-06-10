#pragma once

#include "physics/potential/CoulombPotential.hpp"
#include "physics/potential/GeneralPotentialExpression.hpp"
#include "simulation/model/Potential.hpp"

namespace physics::potential {

template <simulation::model::PotentialType potEval>
auto createPotentialFactory(
    const simulation::model::Potential &pot,
    const std::unordered_map<std::string, math::scalar> &constantsValues) {
    if constexpr (potEval == simulation::model::PotentialType::Coulomb) {
        return [](math::scalar &r) { return CoulombPotential(r); };
    } else if constexpr (potEval ==
                         simulation::model::PotentialType::GeneralExpression) {

        std::string _potential_expression = pot.getExpression();
        return [constantsValues, _potential_expression](math::scalar &r) {
            return GeneralPotentialExpression(r, _potential_expression,
                                              constantsValues);
        };
    }

    static_assert(potEval == simulation::model::PotentialType::Coulomb ||
                      potEval ==
                          simulation::model::PotentialType::GeneralExpression,
                  "Unknown potential type");
}
} // namespace physics::potential
