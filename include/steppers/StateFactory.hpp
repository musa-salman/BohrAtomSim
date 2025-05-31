#ifndef STATE_FACTORY_HPP
#define STATE_FACTORY_HPP

#include "steppers/Potential.hpp"
#include "steppers/PotentialEvaluatorBuilderFactory.hpp"

struct CoulombTag {};
struct GeneralExpressionTag {};

template <typename Tag> struct PotentialTraits;

template <> struct PotentialTraits<CoulombTag> {
    static constexpr PotentialType type = PotentialType::Coulomb;

    static auto make_factory(const Potential &p, const auto &constants) {
        return createPotentialFactory<PotentialType::Coulomb>(p, constants);
    }

    template <typename Factory>
    using PotType = decltype(std::declval<Factory>()(std::declval<scalar &>()));
};

template <> struct PotentialTraits<GeneralExpressionTag> {
    static constexpr PotentialType type = PotentialType::GeneralExpression;

    static auto make_factory(const Potential &p, const auto &constants) {
        return createPotentialFactory<PotentialType::GeneralExpression>(
            p, constants);
    }

    template <typename Factory>
    using PotType = decltype(std::declval<Factory>()(std::declval<scalar &>()));
};

#endif // STATE_FACTORY_HPP