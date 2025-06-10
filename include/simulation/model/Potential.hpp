#pragma once

#include <cstddef>
#include <exprtk.hpp>
#include <nlohmann/json.hpp>

#include "physics/math/types.hpp"

namespace simulation::model {

enum class PotentialType { Coulomb, GeneralExpression };

class Potential {
  private:
    size_t id;
    std::string name;
    PotentialType type;
    std::string expression_str;
    std::unordered_map<std::string, physics::math::scalar> constants;

  public:
    Potential();
    Potential(size_t id, const std::string &name, PotentialType type,
              const std::string &expression_str,
              const std::unordered_map<std::string, physics::math::scalar>
                  &constants);
    Potential(const Potential &other);
    Potential &operator=(const Potential &other);

    size_t getId() const;
    void setId(size_t new_id);

    const std::string &getName() const;
    void setName(const std::string &new_name);

    PotentialType getType() const;
    void setType(PotentialType new_type);

    const std::string &getExpression() const;
    void setExpression(const std::string &new_expression);

    const std::unordered_map<std::string, physics::math::scalar> &
    getConstants() const;

    void
    setConstants(const std::unordered_map<std::string, physics::math::scalar>
                     &new_constants);

    void setConstants(const nlohmann::json &new_constants);

    std::string serializeConstants() const;
};
} // namespace simulation::model