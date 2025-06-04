#ifndef POTENTIAL_HPP
#define POTENTIAL_HPP

#include <cstddef>
#include <nlohmann/json.hpp>

#include "eom/utils.hpp"
#include "exprtk.hpp"

enum class PotentialType { Coulomb, GeneralExpression };

class Potential {
  private:
    size_t id;
    std::string name;
    PotentialType type;
    std::string expression_str;
    std::unordered_map<std::string, scalar> constants;

  public:
    Potential();
    Potential(size_t id, const std::string &name, PotentialType type,
              const std::string &expression_str,
              const std::unordered_map<std::string, scalar> &constants);
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

    const std::unordered_map<std::string, scalar> &getConstants() const;

    void
    setConstants(const std::unordered_map<std::string, scalar> &new_constants);

    void setConstants(const nlohmann::json &new_constants);

    std::string serializeConstants() const;
};

struct PotentialEvaluator {
    SIM_CONST SIM_INLINE inline scalar dU_dr(this const auto &self) {
        return self.dU_dr();
    }
};

struct CoulombPotential : PotentialEvaluator {
    CoulombPotential(scalar &r) : m_r(r) {}

    CoulombPotential &operator=(const CoulombPotential &other) {
        if (this != &other)
            m_r = other.m_r;
        return *this;
    }

    CoulombPotential(const CoulombPotential &other) : m_r(other.m_r) {}

    CoulombPotential rebind(scalar &new_r) const {
        return CoulombPotential(new_r);
    }

    SIM_CONST SIM_INLINE inline scalar dU_dr() const {
        const scalar result = 1.0 / (m_r * m_r);
        return result;
    }

  private:
    scalar &m_r;
};

struct GeneralPotentialExpression : PotentialEvaluator {

    GeneralPotentialExpression(
        scalar &r, const std::string &expression,
        const std::unordered_map<std::string, scalar> &constantsValues)
        : m_constants_values(constantsValues), m_expression(expression) {
        exprtk::parser<scalar> parser;

        exprtk::symbol_table<scalar> m_symbol_table;
        for (const auto &pair : m_constants_values) {
            m_symbol_table.add_constant(pair.first, pair.second);
        }
        m_symbol_table.add_variable("r", r);

        m_du_dr.register_symbol_table(m_symbol_table);

        parser.compile(m_expression, m_du_dr);
    }

    GeneralPotentialExpression(const GeneralPotentialExpression &other)
        : m_du_dr(other.m_du_dr), m_constants_values(other.m_constants_values),
          m_expression(other.m_expression) {}

    SIM_CONST SIM_INLINE inline scalar dU_dr() const { return m_du_dr.value(); }

    GeneralPotentialExpression rebind(scalar &new_r) const {
        return GeneralPotentialExpression(new_r, m_expression,
                                          m_constants_values);
    }

  private:
    exprtk::expression<scalar> m_du_dr;
    const std::unordered_map<std::string, scalar> m_constants_values;
    const std::string m_expression;
};

#endif // POTENTIAL_HPP