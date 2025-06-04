#ifndef POTENTIAL_EVALUATOR_BUILDER_FACTORY_HPP
#define POTENTIAL_EVALUATOR_BUILDER_FACTORY_HPP

#include <expected>

#include "steppers/Potential.hpp"

template <PotentialType potEval>
auto createPotentialFactory(
    const Potential &pot,
    const std::unordered_map<std::string, scalar> &constantsValues) {
    if constexpr (potEval == PotentialType::Coulomb) {
        return [](scalar &r) { return CoulombPotential(r); };
    } else if constexpr (potEval == PotentialType::GeneralExpression) {

        std::string _potential_expression = pot.getExpression();
        return [constantsValues, _potential_expression](scalar &r) {
            return GeneralPotentialExpression(r, _potential_expression,
                                              constantsValues);
        };
    }

    static_assert(potEval == PotentialType::Coulomb ||
                      potEval == PotentialType::GeneralExpression,
                  "Unknown potential type");
}

#endif // POTENTIAL_EVALUATOR_BUILDER_FACTORY_HPP
