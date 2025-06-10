#include "simulation/model/Potential.hpp"

namespace simulation::model {

using namespace physics::math;

Potential::Potential()
    : id(0), type(PotentialType::Coulomb), expression_str("-(q/r)^2") {}

Potential::Potential(size_t id, const std::string &name, PotentialType type,
                     const std::string &expression_str,
                     const std::unordered_map<std::string, scalar> &constants)
    : id(id), name(name), type(type), expression_str(expression_str),
      constants(constants) {}

Potential::Potential(const Potential &other)
    : id(other.id), name(other.name), type(other.type),
      expression_str(other.expression_str), constants(other.constants) {}

Potential &Potential::operator=(const Potential &other) {
    if (this != &other) {
        id = other.id;
        name = other.name;
        type = other.type;
        expression_str = other.expression_str;
        constants = other.constants;
    }
    return *this;
}

size_t Potential::getId() const { return id; }

void Potential::setId(size_t new_id) { id = new_id; }

const std::string &Potential::getName() const { return name; }

void Potential::setName(const std::string &new_name) { name = new_name; }

PotentialType Potential::getType() const { return type; }

void Potential::setType(PotentialType new_type) { type = new_type; }

const std::string &Potential::getExpression() const { return expression_str; }

void Potential::setExpression(const std::string &new_expression) {
    expression_str = new_expression;
}

const std::unordered_map<std::string, scalar> &Potential::getConstants() const {
    return constants;
}

void Potential::setConstants(
    const std::unordered_map<std::string, scalar> &new_constants) {
    constants = new_constants;
}

void Potential::setConstants(const nlohmann::json &new_constants) {
    constants.clear();
    if (new_constants.size() == 0) {
        return; // No constants to set
    }

    for (const auto &[key, value] : new_constants.items()) {
        constants[key] = value.get<scalar>();
    }
}

std::string Potential::serializeConstants() const {
    nlohmann::json params;
    for (const auto &[key, value] : constants) {
        params[key] = value;
    }
    return params.dump();
}
} // namespace simulation::model