
#include "simulation/factories/PotentialBuilder.hpp"
#include "simulation/model/Potential.hpp"

namespace simulation::factories {

using namespace simulation::model;
using namespace physics::math;

void PotentialBuilder::setPotentialType(PotentialType type) {
    potential_type = type;
}

void PotentialBuilder::setPotentialExpression(const std::string &expression) {
    if (expression.empty()) {
        // return std::unexpected("Potential expression cannot be empty.");
    }
    potentialExpression = expression;
    // return {};
}

void PotentialBuilder::addPotentialConstant(const std::string &name,
                                            scalar value) {
    potentialConstants[name] = value;
}

void PotentialBuilder::removePotentialConstant(const std::string &name) {
    potentialConstants.erase(name);
}

void PotentialBuilder::clearPotentialConstants() { potentialConstants.clear(); }

const std::unordered_map<std::string, scalar> &
PotentialBuilder::getPotentialConstants() const {
    return potentialConstants;
}

std::string PotentialBuilder::getPotentialExpression() const {
    return potentialExpression;
}

PotentialType PotentialBuilder::getPotentialType() const {
    return potential_type;
}

PotentialBuilder::PotentialBuilder() : potential_type(PotentialType::Coulomb) {}

Potential PotentialBuilder::build() const {
    Potential potential;
    potential.setType(potential_type);
    potential.setExpression(potentialExpression);
    potential.setConstants(potentialConstants);
    return potential;
}
} // namespace simulation::factories