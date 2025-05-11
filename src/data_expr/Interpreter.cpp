#include <cmath>
#include <memory>
#include <variant>
#include <vector>

#include "data_expr/Expr.hpp"
#include "data_expr/Interpreter.hpp"
#include "data_expr/Token.hpp"
#include "dataset/BitVector.hpp"
#include "dataset/VectorOperations.hpp"

using EvalResult = std::variant<std::unique_ptr<std::vector<double>>,
                                std::unique_ptr<BitVector>>;

Interpreter::Interpreter(const Dataset &dataset) : dataset(dataset) {}

EvalResult Interpreter::evaluate(const std::shared_ptr<Expr> &expr) {
    expr->accept(*this);

    auto result = std::move(resultStack.top());
    resultStack.pop();
    return result;
}

void Interpreter::visitTrinaryExpr(const Trinary &expr) {
    auto left = evaluate(expr.left);
    auto middle = evaluate(expr.middle);
    auto right = evaluate(expr.right);

    if (std::holds_alternative<std::unique_ptr<std::vector<double>>>(left) &&
        std::holds_alternative<std::unique_ptr<std::vector<double>>>(middle) &&
        std::holds_alternative<std::unique_ptr<std::vector<double>>>(right) &&
        expr.op1.type == TokenType::APPROX &&
        expr.op2.type == TokenType::COLON) {
        const auto &lv = std::get<std::unique_ptr<std::vector<double>>>(left);
        const auto &mv = std::get<std::unique_ptr<std::vector<double>>>(middle);
        const auto &rv = std::get<std::unique_ptr<std::vector<double>>>(right);

        resultStack.push(std::make_unique<BitVector>(
            VectorOperations::approx(*lv, *mv, *rv)));
        return;
    }

    throw std::runtime_error("Invalid operand types for trinary operation");
}

void Interpreter::visitVariableExpr(const Variable &expr) {
    auto it = dataset.get(expr.name.lexeme);
    resultStack.push(std::make_unique<std::vector<double>>(std::move(it)));
}

void Interpreter::visitBinaryExpr(const Binary &expr) {
    auto left = evaluate(expr.left);
    auto right = evaluate(expr.right);

    if ((std::holds_alternative<std::unique_ptr<std::vector<double>>>(left) &&
         std::holds_alternative<std::unique_ptr<BitVector>>(right)) ||
        (std::holds_alternative<std::unique_ptr<BitVector>>(left) &&
         std::holds_alternative<std::unique_ptr<std::vector<double>>>(right))) {
        throw std::runtime_error("Invalid operand types for binary operation");
    }

    if (std::holds_alternative<std::unique_ptr<std::vector<double>>>(left) &&
        std::holds_alternative<std::unique_ptr<std::vector<double>>>(right)) {
        const auto &lv = std::get<std::unique_ptr<std::vector<double>>>(left);
        const auto &rv = std::get<std::unique_ptr<std::vector<double>>>(right);

        if (expr.op.type == TokenType::PLUS) {
            resultStack.push(std::make_unique<std::vector<double>>(
                VectorOperations::add(*lv, *rv)));
            return;
        } else if (expr.op.type == TokenType::MINUS) {
            resultStack.push(std::make_unique<std::vector<double>>(
                VectorOperations::subtract(*lv, *rv)));
            return;
        } else if (expr.op.type == TokenType::STAR) {
            resultStack.push(std::make_unique<std::vector<double>>(
                VectorOperations::multiply(*lv, *rv)));
            return;
        } else if (expr.op.type == TokenType::SLASH) {
            resultStack.push(std::make_unique<std::vector<double>>(
                VectorOperations::divide(*lv, *rv)));
            return;
        } else if (expr.op.type == TokenType::CARET) {
            resultStack.push(std::make_unique<std::vector<double>>(
                VectorOperations::power(*lv, *rv)));
            return;
        } else if (expr.op.type == TokenType::GREATER) {
            resultStack.push(std::make_unique<BitVector>(
                VectorOperations::greater(*lv, *rv)));
            return;
        } else if (expr.op.type == TokenType::LESS) {
            resultStack.push(
                std::make_unique<BitVector>(VectorOperations::less(*lv, *rv)));
            return;
        } else if (expr.op.type == TokenType::EQUAL) {
            resultStack.push(
                std::make_unique<BitVector>(VectorOperations::equal(*lv, *rv)));
            return;
        } else if (expr.op.type == TokenType::BANG_EQUAL) {
            resultStack.push(std::make_unique<BitVector>(
                VectorOperations::notEqual(*lv, *rv)));
            return;
        } else if (expr.op.type == TokenType::APPROX) {
            resultStack.push(
                std::make_unique<BitVector>(VectorOperations::approx(
                    *lv, *rv, std::vector<double>(rv->size(), 1e-4))));
            return;
        }

        else {
            throw std::runtime_error("Unknown binary operator");
        }
    }

    const auto &bv = std::get<std::unique_ptr<BitVector>>(left);
    const auto &rv = std::get<std::unique_ptr<BitVector>>(right);

    if (expr.op.type == TokenType::AND) {
        resultStack.push(
            std::make_unique<BitVector>(VectorOperations::andOp(*bv, *rv)));
        return;
    } else if (expr.op.type == TokenType::OR) {
        resultStack.push(
            std::make_unique<BitVector>(VectorOperations::orOp(*bv, *rv)));
        return;
    } else if (expr.op.type == TokenType::BANG) {
        resultStack.push(
            std::make_unique<BitVector>(VectorOperations::notOp(*bv)));
        return;
    } else
        throw std::runtime_error("Unknown binary operator");
}

void Interpreter::visitUnaryExpr(const Unary &expr) {
    auto val = evaluate(expr.right);

    if (expr.op.type == TokenType::MINUS &&
        std::holds_alternative<std::unique_ptr<std::vector<double>>>(val)) {
        const auto &v = std::get<std::unique_ptr<std::vector<double>>>(val);
        resultStack.push(std::make_unique<std::vector<double>>(
            VectorOperations::multiply(*v, -1)));
        return;
    } else if (expr.op.type == TokenType::BANG &&
               std::holds_alternative<std::unique_ptr<BitVector>>(val)) {
        const auto &bv = std::get<std::unique_ptr<BitVector>>(val);
        resultStack.push(
            std::make_unique<BitVector>(VectorOperations::notOp(*bv)));
        return;
    }

    throw std::runtime_error("Unknown unary operator");
}

void Interpreter::visitCallExpr(const Call &expr) {
    if (expr.callee.lexeme == "sin") {
        auto arg = evaluate(expr.args[0]);
        if (std::holds_alternative<std::unique_ptr<std::vector<double>>>(arg)) {
            const auto &v = std::get<std::unique_ptr<std::vector<double>>>(arg);
            resultStack.push(std::make_unique<std::vector<double>>(
                VectorOperations::sin(*v)));
            return;
        }
    } else if (expr.callee.lexeme == "cos") {
        auto arg = evaluate(expr.args[0]);
        if (std::holds_alternative<std::unique_ptr<std::vector<double>>>(arg)) {
            const auto &v = std::get<std::unique_ptr<std::vector<double>>>(arg);
            resultStack.push(std::make_unique<std::vector<double>>(
                VectorOperations::cos(*v)));
            return;
        }
    } else if (expr.callee.lexeme == "tan") {
        auto arg = evaluate(expr.args[0]);
        if (std::holds_alternative<std::unique_ptr<std::vector<double>>>(arg)) {
            const auto &v = std::get<std::unique_ptr<std::vector<double>>>(arg);
            resultStack.push(std::make_unique<std::vector<double>>(
                VectorOperations::tan(*v)));
            return;
        }
    }

    throw std::runtime_error("Unknown function call");
}

void Interpreter::visitGroupingExpr(const Grouping &expr) {
    resultStack.push(evaluate(expr.expression));
}

void Interpreter::visitNumberExpr(const Number &expr) {
    std::vector<double> result(dataset.getRowCount());
    const double value = std::any_cast<double>(expr.value.literal);
    result.assign(dataset.getRowCount(), value);
    resultStack.push(std::make_unique<std::vector<double>>(std::move(result)));
}