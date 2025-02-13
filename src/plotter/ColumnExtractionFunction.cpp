#include "plotter/ColumnExtractionFunction.hpp"
#include <unordered_map>

ColumnExtractionFunction::ColumnExtractionFunction(
    const std::string &columnName)
    : columnName(columnName) {}

static const std::unordered_map<std::string, std::string> colName2latex = {
    {"r", "r"},       {"r_dot", "r'"},     {"r_dot_dot", "r''"},
    {"theta", "θ"},   {"theta_dot", "θ'"}, {"theta_dot_dot", "θ''"},
    {"phi", "φ"},     {"phi_dot", "φ'"},   {"psi", "Ψ"},
    {"psi_dot", "Ψ'"}};

std::string ColumnExtractionFunction::name() const { return columnName; }

std::string ColumnExtractionFunction::latexName() const {
    return colName2latex.at(columnName);
}

std::optional<std::vector<double>> ColumnExtractionFunction::apply(
    const std::shared_ptr<std::unordered_map<std::string, std::vector<double>>>
        &data) const {
    if (!data)
        return std::nullopt;

    if (auto it = data->find(columnName); it != data->end()) {
        return it->second;
    }
    return std::nullopt;
}
