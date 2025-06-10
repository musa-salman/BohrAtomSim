#pragma once

#include "physics/math/types.hpp"
#include "simulation/model/Potential.hpp"

namespace simulation::factories {

class PotentialBuilder {

    model::PotentialType potential_type;

    std::string potentialExpression;
    std::unordered_map<std::string, physics::math::scalar> potentialConstants;

  public:
    explicit PotentialBuilder();

    void setPotentialType(model::PotentialType type);

    void setPotentialExpression(const std::string &expression);

    void addPotentialConstant(const std::string &name,
                              physics::math::scalar value);
    void removePotentialConstant(const std::string &name);
    void clearPotentialConstants();

    const std::unordered_map<std::string, physics::math::scalar> &
    getPotentialConstants() const;
    std::string getPotentialExpression() const;

    model::PotentialType getPotentialType() const;

    model::Potential build() const;
};
} // namespace simulation::factories
