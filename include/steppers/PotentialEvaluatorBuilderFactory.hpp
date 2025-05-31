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
        exprtk::symbol_table<scalar> symbol_table;

        for (auto &pair : constantsValues)
            symbol_table.add_constant(pair.first, pair.second);

        std::string _potential_expression = pot.getExpression();
        return [symbol_table, _potential_expression](scalar &r) mutable {
            symbol_table.add_variable("r", r);
            return GeneralPotentialExpression(_potential_expression,
                                              symbol_table);
        };
    }

    static_assert(potEval == PotentialType::Coulomb ||
                      potEval == PotentialType::GeneralExpression,
                  "Unknown potential type");
}

#endif // POTENTIAL_EVALUATOR_BUILDER_FACTORY_HPP
