#pragma once

#include "PotentialEvaluatorBuilderFactory.hpp"
#include "simulation/model/Potential.hpp"

namespace physics::potential {

struct CoulombTag {};
struct GeneralExpressionTag {};

template <typename Tag> struct PotentialTraits;

template <> struct PotentialTraits<CoulombTag> {
    static constexpr simulation::model::PotentialType type =
        simulation::model::PotentialType::Coulomb;

    static auto make_factory(const simulation::model::Potential &p,
                             const auto &constants) {
        return createPotentialFactory<
            simulation::model::PotentialType::Coulomb>(p, constants);
    }

    template <typename Factory>
    using PotType =
        decltype(std::declval<Factory>()(std::declval<math::scalar &>()));
};

template <> struct PotentialTraits<GeneralExpressionTag> {
    static constexpr simulation::model::PotentialType type =
        simulation::model::PotentialType::GeneralExpression;

    static auto make_factory(const simulation::model::Potential &p,
                             const auto &constants) {
        return createPotentialFactory<
            simulation::model::PotentialType::GeneralExpression>(p, constants);
    }

    template <typename Factory>
    using PotType =
        decltype(std::declval<Factory>()(std::declval<math::scalar &>()));
};
} // namespace physics::potential
