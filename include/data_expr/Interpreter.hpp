#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include <memory>
#include <variant>
#include <vector>

#include "data_expr/Expr.hpp"
#include "dataset/BitVector.hpp"
#include "dataset/Dataset.hpp"

class Interpreter
    : public Expr::Visitor<std::variant<std::vector<double>, BitVector>> {
  public:
    explicit Interpreter(Dataset &dataset);

    [[nodiscard]] std::variant<std::vector<double>, BitVector>
    evaluate(const std::shared_ptr<Expr> &expr);

    std::variant<std::vector<double>, BitVector>
    visitBinaryExpr(const Binary &expr) override;

    std::variant<std::vector<double>, BitVector>
    visitUnaryExpr(const Unary &expr) override;

    std::variant<std::vector<double>, BitVector>
    visitGroupingExpr(const Grouping &expr) override;

    std::variant<std::vector<double>, BitVector>
    visitNumberExpr(const Number &expr) override;

    std::variant<std::vector<double>, BitVector>
    visitVariableExpr(const Variable &expr) override;

    std::variant<std::vector<double>, BitVector>
    visitCallExpr(const Call &expr) override;

  private:
    const Dataset &dataset;
};

#endif