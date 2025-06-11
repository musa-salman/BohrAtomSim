
#include "simulation/factories/PotentialBuilder.hpp"
#include "simulation/model/Potential.hpp"

namespace simulation::factories {

using namespace simulation::model;
using namespace physics::math;

void PotentialBuilder::setName(const std::string &name) { m_name = name; }

const std::string &PotentialBuilder::getName() const { return m_name; }

void PotentialBuilder::setPotentialType(PotentialType type) {
    m_potentialType = type;
}

void PotentialBuilder::setPotentialExpression(const std::string &expression) {
    if (expression.empty()) {
        // return std::unexpected("Potential expression cannot be empty.");
    }
    m_potentialExpression = expression;
    // return {};
}

void PotentialBuilder::addPotentialConstant(const std::string &name,
                                            scalar value) {
    m_potentialConstants[name] = value;
}

void PotentialBuilder::removePotentialConstant(const std::string &name) {
    m_potentialConstants.erase(name);
}

void PotentialBuilder::clearPotentialConstants() {
    m_potentialConstants.clear();
}

const std::unordered_map<std::string, scalar> &
PotentialBuilder::getPotentialConstants() const {
    return m_potentialConstants;
}

std::string PotentialBuilder::getPotentialExpression() const {
    return m_potentialExpression;
}

PotentialType PotentialBuilder::getPotentialType() const {
    return m_potentialType;
}

PotentialBuilder::PotentialBuilder()
    : m_potentialType(PotentialType::Coulomb) {}

Potential PotentialBuilder::build() const {
    Potential potential;
    potential.setName(m_name);
    potential.setType(m_potentialType);
    potential.setExpression(m_potentialExpression);
    potential.setConstants(m_potentialConstants);
    return potential;
}
} // namespace simulation::factories