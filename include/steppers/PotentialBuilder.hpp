#ifndef POTENTIAL_BUILDER_HPP
#define POTENTIAL_BUILDER_HPP

#include <expected>

#include "steppers/Potential.hpp"

class PotentialBuilder {

    PotentialType potential_type;

    std::string potential_expression;
    std::unordered_map<std::string, scalar> potential_constants;

  public:
    explicit PotentialBuilder();

    void setPotentialType(PotentialType type);

    void setPotentialExpression(const std::string &expression);

    void addPotentialConstant(const std::string &name, scalar value);
    void removePotentialConstant(const std::string &name);
    void clearPotentialConstants();

    const std::unordered_map<std::string, scalar> &
    getPotentialConstants() const;
    std::string getPotentialExpression() const;

    PotentialType getPotentialType() const;

    Potential build() const;
};

#endif // POTENTIAL_BUILDER_HPP