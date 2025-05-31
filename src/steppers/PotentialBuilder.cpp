#include "steppers/PotentialBuilder.hpp"

void PotentialBuilder::setPotentialType(PotentialType type) {
    potential_type = type;
}

void PotentialBuilder::setPotentialExpression(const std::string &expression) {
    if (expression.empty()) {
        // return std::unexpected("Potential expression cannot be empty.");
    }
    potential_expression = expression;
    // return {};
}

void PotentialBuilder::addPotentialConstant(const std::string &name,
                                            scalar value) {
    potential_constants[name] = value;
}

void PotentialBuilder::removePotentialConstant(const std::string &name) {
    potential_constants.erase(name);
}

void PotentialBuilder::clearPotentialConstants() {
    potential_constants.clear();
}

const std::unordered_map<std::string, scalar> &
PotentialBuilder::getPotentialConstants() const {
    return potential_constants;
}

std::string PotentialBuilder::getPotentialExpression() const {
    return potential_expression;
}

PotentialType PotentialBuilder::getPotentialType() const {
    return potential_type;
}

PotentialBuilder::PotentialBuilder() : potential_type(PotentialType::Coulomb) {}

Potential PotentialBuilder::build() const {
    Potential potential;
    potential.setType(potential_type);
    potential.setExpression(potential_expression);
    potential.setConstants(potential_constants);
    return potential;
}