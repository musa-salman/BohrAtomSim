#ifndef POTENTIAL_HPP
#define POTENTIAL_HPP

#include <cstddef>
#include <iostream>
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
        std::string expression,
        const exprtk::symbol_table<scalar> &symbol_table) {
        exprtk::parser<scalar> parser;
        m_expression = std::move(expression);
        m_symbol_table = symbol_table;

        m_du_dr.register_symbol_table(m_symbol_table);

        parser.compile(m_expression, m_du_dr);
    }

    GeneralPotentialExpression &
    operator=(const GeneralPotentialExpression &other) {
        if (this != &other) {
            m_expression = other.m_expression;
            m_symbol_table = other.m_symbol_table;
            m_du_dr = other.m_du_dr;

            exprtk::parser<scalar> parser;
            parser.compile(m_expression, m_du_dr);
        }
        return *this;
    }

    GeneralPotentialExpression(const GeneralPotentialExpression &other)
        : m_du_dr(std::move(other.m_du_dr)),
          m_symbol_table(std::move(other.m_symbol_table)),
          m_expression(std::move(other.m_expression)) {
        exprtk::parser<scalar> parser;
        parser.compile(m_expression, m_du_dr);
    }

    ~GeneralPotentialExpression() {
        std::cout << "GeneralPotentialExpression destroyed" << std::endl;
    }

    SIM_CONST SIM_INLINE inline scalar dU_dr() const { return m_du_dr.value(); }

    GeneralPotentialExpression rebind(scalar &new_r) const {
        exprtk::symbol_table<scalar> symbol_table = m_symbol_table;
        exprtk::expression<scalar> du_dr;
        symbol_table.add_variable("r", new_r);
        du_dr.register_symbol_table(symbol_table);
        return GeneralPotentialExpression(m_expression, m_symbol_table);
    }

  private:
    exprtk::expression<scalar> m_du_dr;
    exprtk::symbol_table<scalar> m_symbol_table;
    std::string m_expression;
};

#endif // POTENTIAL_HPP