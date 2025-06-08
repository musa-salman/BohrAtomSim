#include <exprtk.hpp>
#include <iostream>
#include <limits>
#include <unordered_map>

#include "dataset/Dataset.hpp"
#include "dataset/expression_utils.hpp"

namespace dataset {

std::vector<double> computeVectorFromExpression(
    const std::string &expr, const Dataset &dataset,
    const boost::container::flat_map<std::string, double> &constants) {

    using T = double;
    using symbol_table_t = exprtk::symbol_table<T>;
    using expression_t = exprtk::expression<T>;
    using parser_t = exprtk::parser<T>;

    const size_t rowCount = dataset.getRowCount();
    const auto &columnNames = dataset.getColumnsNames();

    symbol_table_t symbolTable;
    std::unordered_map<std::string, T> variables;

    // Bind scalar variables to the symbol table
    for (const auto &name : columnNames) {
        variables[name] = T{0};
        symbolTable.add_variable(name, variables[name]);
    }

    for (const auto &pair : constants) {
        symbolTable.add_constant(pair.first, pair.second);
    }

    expression_t expression;
    expression.register_symbol_table(symbolTable);

    parser_t parser;
    if (!parser.compile(expr, expression)) {
        std::cerr << "[exprtk] Failed to parse expression: " << expr << "\n";
        std::cerr << "Error: " << parser.error() << "\n";
        return std::vector<T>(rowCount, std::numeric_limits<T>::quiet_NaN());
    }

    std::vector<T> result(rowCount);

    for (size_t i = 0; i < rowCount; ++i) {
        for (const auto &name : columnNames) {
            variables[name] = dataset.get(name)[i];
        }

        result[i] = expression.value();
    }

    return result;
}

boost::dynamic_bitset<> computeMaskFromExpression(const std::string &filterExpr,
                                                  const Dataset &dataset) {
    using T = double;
    using symbol_table_t = exprtk::symbol_table<T>;
    using expression_t = exprtk::expression<T>;
    using parser_t = exprtk::parser<T>;

    const size_t rowCount = dataset.getRowCount();
    const auto &columnNames = dataset.getColumnsNames();

    symbol_table_t symbolTable;
    std::unordered_map<std::string, T> variables;

    for (const auto &name : columnNames) {
        variables[name] = T{0};
        symbolTable.add_variable(name, variables[name]);
    }

    expression_t expression;
    expression.register_symbol_table(symbolTable);

    parser_t parser;
    if (!parser.compile(filterExpr, expression)) {
        std::cerr << "[exprtk] Failed to parse expression: " << filterExpr
                  << "\n";
        std::cerr << "Error: " << parser.error() << "\n";
        return boost::dynamic_bitset<>(rowCount);
    }

    boost::dynamic_bitset<> mask(rowCount);

    for (size_t i = 0; i < rowCount; ++i) {
        for (const auto &name : columnNames) {
            variables[name] = dataset.get(name)[i];
        }

        if (expression.value()) {
            mask[i] = true;
        }
    }

    return mask;
}

} // namespace dataset
