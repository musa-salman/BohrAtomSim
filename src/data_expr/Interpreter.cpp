
#include <cmath>
#include <variant>
#include <vector>

#include "data_expr/Expr.hpp"
#include "data_expr/Interpreter.hpp"
#include "dataset/BitVector.hpp"
#include "dataset/VectorOperations.hpp"

using EvalResult = std::variant<std::vector<double>, BitVector>;

Interpreter::Interpreter(Dataset &dataset) : dataset(dataset) {}

EvalResult Interpreter::evaluate(const std::shared_ptr<Expr> &expr) {
    return expr->accept(*this);
}

EvalResult Interpreter::visitVariableExpr(const Variable &expr) {
    return dataset.get(expr.name.lexeme);
}

EvalResult Interpreter::visitBinaryExpr(const Binary &expr) {
    auto left = evaluate(expr.left);
    auto right = evaluate(expr.right);

    if ((std::holds_alternative<std::vector<double>>(left) &&
         std::holds_alternative<BitVector>(right)) ||
        (std::holds_alternative<BitVector>(left) &&
         std::holds_alternative<std::vector<double>>(right))) {
        throw std::runtime_error("Invalid operand types for binary operation");
    }

    if (std::holds_alternative<std::vector<double>>(left) &&
        std::holds_alternative<std::vector<double>>(right)) {
        const auto &lv = std::get<std::vector<double>>(left);
        const auto &rv = std::get<std::vector<double>>(right);

        if (expr.op.type == TokenType::PLUS) {
            return VectorOperations::add(lv, rv);
        } else if (expr.op.type == TokenType::MINUS) {
            return VectorOperations::subtract(lv, rv);
        } else if (expr.op.type == TokenType::STAR) {
            return VectorOperations::multiply(lv, rv);
        } else if (expr.op.type == TokenType::SLASH) {
            return VectorOperations::divide(lv, rv);
        } else if (expr.op.type == TokenType::CARET) {
            return VectorOperations::power(lv, rv);
        } else if (expr.op.type == TokenType::GREATER) {
            return VectorOperations::greater(lv, rv);
        } else if (expr.op.type == TokenType::LESS) {
            return VectorOperations::less(lv, rv);
        } else if (expr.op.type == TokenType::EQUAL) {
            return VectorOperations::equal(lv, rv);
        } else if (expr.op.type == TokenType::BANG_EQUAL) {
            return VectorOperations::notEqual(lv, rv);
        } else {
            throw std::runtime_error("Unknown binary operator");
        }
    }

    const auto &bv = std::get<BitVector>(left);
    const auto &rv = std::get<BitVector>(right);

    if (expr.op.type == TokenType::AND) {
        return VectorOperations::andOp(bv, rv);
    } else if (expr.op.type == TokenType::OR) {
        return VectorOperations::orOp(bv, rv);
    } else if (expr.op.type == TokenType::BANG) {
        return VectorOperations::notOp(bv);
    } else {
        throw std::runtime_error("Unknown binary operator");
    }
}

EvalResult Interpreter::visitUnaryExpr(const Unary &expr) {
    auto val = evaluate(expr.right);

    if (expr.op.type == TokenType::MINUS &&
        std::holds_alternative<std::vector<double>>(val)) {
        const auto &v = std::get<std::vector<double>>(val);
        return VectorOperations::multiply(v, -1);
    } else if (expr.op.type == TokenType::BANG &&
               std::holds_alternative<BitVector>(val)) {
        const auto &bv = std::get<BitVector>(val);
        return VectorOperations::notOp(bv);
    }

    throw std::runtime_error("Unknown unary operator");
}

EvalResult Interpreter::visitCallExpr(const Call &expr) {
    if (expr.callee.lexeme == "sin") {
        auto arg = evaluate(expr.args[0]);
        if (std::holds_alternative<std::vector<double>>(arg)) {
            const auto &v = std::get<std::vector<double>>(arg);
            return VectorOperations::sin(v);
        }
    } else if (expr.callee.lexeme == "cos") {
        auto arg = evaluate(expr.args[0]);
        if (std::holds_alternative<std::vector<double>>(arg)) {
            const auto &v = std::get<std::vector<double>>(arg);
            return VectorOperations::cos(v);
        }
    } else if (expr.callee.lexeme == "tan") {
        auto arg = evaluate(expr.args[0]);
        if (std::holds_alternative<std::vector<double>>(arg)) {
            const auto &v = std::get<std::vector<double>>(arg);
            return VectorOperations::tan(v);
        }
    }

    throw std::runtime_error("Unknown function call");
}

EvalResult Interpreter::visitGroupingExpr(const Grouping &expr) {
    return evaluate(expr.expression);
}

EvalResult Interpreter::visitNumberExpr(const Number &expr) {
    std::vector<double> result(dataset.getRowCount());
    const double value = std::any_cast<double>(expr.value.literal);
    result.assign(dataset.getRowCount(), value);
    return result;
}