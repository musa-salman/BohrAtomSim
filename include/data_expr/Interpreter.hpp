#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include <memory>
#include <stack>
#include <variant>
#include <vector>

#include "data_expr/Expr.hpp"
#include "dataset/BitVector.hpp"
#include "dataset/Dataset.hpp"

class Interpreter : public Expr::Visitor {
  public:
    explicit Interpreter(const Dataset &dataset);

    [[nodiscard]] std::variant<std::unique_ptr<std::vector<double>>,
                               std::unique_ptr<BitVector>>
    evaluate(const std::shared_ptr<Expr> &expr);

    void visitTrinaryExpr(const Trinary &expr) override;

    void visitBinaryExpr(const Binary &expr) override;

    void visitUnaryExpr(const Unary &expr) override;

    void visitGroupingExpr(const Grouping &expr) override;

    void visitNumberExpr(const Number &expr) override;

    void visitVariableExpr(const Variable &expr) override;

    void visitCallExpr(const Call &expr) override;

  private:
    const Dataset &dataset;

    std::stack<std::variant<std::unique_ptr<std::vector<double>>,
                            std::unique_ptr<BitVector>>>
        resultStack;
};

#endif // INTERPRETER_HPP