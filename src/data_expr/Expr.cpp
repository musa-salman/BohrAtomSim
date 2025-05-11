#include "data_expr/Expr.hpp"

Trinary::Trinary(std::shared_ptr<Expr> left, Token op1,
                 std::shared_ptr<Expr> middle, Token op2,
                 std::shared_ptr<Expr> right)
    : left(left), op1(op1), middle(middle), op2(op2), right(right) {}

void Trinary::accept(Visitor &visitor) { visitor.visitTrinaryExpr(*this); }

Binary::Binary(std::shared_ptr<Expr> left, Token op,
               std::shared_ptr<Expr> right)
    : left(left), op(op), right(right) {}

void Binary::accept(Visitor &visitor) { visitor.visitBinaryExpr(*this); }

Unary::Unary(Token op, std::shared_ptr<Expr> right) : op(op), right(right) {}

void Unary::accept(Visitor &visitor) { visitor.visitUnaryExpr(*this); }

Grouping::Grouping(std::shared_ptr<Expr> expression) : expression(expression) {}

void Grouping::accept(Visitor &visitor) { visitor.visitGroupingExpr(*this); }

Number::Number(Token value) : value(value) {}
void Number::accept(Visitor &visitor) { visitor.visitNumberExpr(*this); }

Variable::Variable(Token name) : name(name) {}

void Variable::accept(Visitor &visitor) { visitor.visitVariableExpr(*this); }

Call::Call(Token callee, std::vector<std::shared_ptr<Expr>> args)
    : callee(callee), args(args) {}

void Call::accept(Visitor &visitor) { visitor.visitCallExpr(*this); }