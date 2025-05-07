
#include "data_expr/Expr.hpp"
#include "data_expr/AstTypeChecker.hpp"

template <typename R> R Expr::accept(Visitor<R> &visitor) {
    return acceptImpl(visitor);
}

Binary::Binary(std::shared_ptr<Expr> left, Token op,
               std::shared_ptr<Expr> right)
    : left(left), op(op), right(right) {}

template <typename R> R Binary::acceptImpl(Visitor<R> &visitor) const {
    return visitor.visitBinaryExpr(*this);
}

Unary::Unary(Token op, std::shared_ptr<Expr> right) : op(op), right(right) {}

template <typename R> R Unary::acceptImpl(Visitor<R> &visitor) const {
    return visitor.visitUnaryExpr(*this);
}

Grouping::Grouping(std::shared_ptr<Expr> expression) : expression(expression) {}

template <typename R> R Grouping::acceptImpl(Visitor<R> &visitor) const {
    return visitor.visitGroupingExpr(*this);
}

template <typename R> R Number::acceptImpl(Visitor<R> &visitor) const {
    return visitor.visitNumberExpr(*this);
}

Variable::Variable(Token name) : name(name) {}

template <typename R> R Variable::acceptImpl(Visitor<R> &visitor) const {
    return visitor.visitVariableExpr(*this);
}

Call::Call(Token callee, std::vector<std::shared_ptr<Expr>> args)
    : callee(callee), args(args) {}

template <typename R> R Call::acceptImpl(Visitor<R> &visitor) const {
    return visitor.visitCallExpr(*this);
}