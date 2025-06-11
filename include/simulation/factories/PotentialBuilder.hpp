#pragma once

#include "physics/math/types.hpp"
#include "simulation/model/Potential.hpp"

namespace simulation::factories {

class PotentialBuilder {
  public:
    explicit PotentialBuilder();

    void setName(const std::string &name);
    const std::string &getName() const;

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

  private:
    model::PotentialType m_potentialType;
    std::string m_name;

    std::string m_potentialExpression;
    std::unordered_map<std::string, physics::math::scalar> m_potentialConstants;
};
} // namespace simulation::factories
