#pragma once

#include <exprtk.hpp>
#include <string>
#include <unordered_map>

#include "physics/math/macros.hpp"
#include "physics/math/types.hpp"
#include "physics/potential/PotentialEvaluator.hpp"

namespace physics::potential {

struct GeneralPotentialExpression : PotentialEvaluator {
    GeneralPotentialExpression(
        math::scalar &r, const std::string &expression,
        const std::unordered_map<std::string, math::scalar> &constantsValues)
        : m_constants_values(constantsValues), m_expression(expression) {
        exprtk::parser<math::scalar> parser;

        exprtk::symbol_table<math::scalar> m_symbol_table;
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

    SIM_CONST SIM_INLINE math::scalar dU_dr() const noexcept {
        return m_du_dr.value();
    }

    GeneralPotentialExpression rebind(math::scalar &new_r) const {
        return GeneralPotentialExpression(new_r, m_expression,
                                          m_constants_values);
    }

  private:
    exprtk::expression<math::scalar> m_du_dr;
    const std::unordered_map<std::string, math::scalar> m_constants_values;
    const std::string m_expression;
};

} // namespace physics::potential