#ifndef EXPR_HPP
#define EXPR_HPP

#include <memory>
#include <vector>

#include "data_expr/Token.hpp"

struct Binary;
struct Call;
struct Grouping;
struct Number;
struct Unary;
struct Variable;

struct Expr {
    virtual ~Expr() = default;

    template <typename R> struct Visitor {
        virtual R visitBinaryExpr(const Binary &expr) = 0;
        virtual R visitCallExpr(const Call &expr) = 0;
        virtual R visitGroupingExpr(const Grouping &expr) = 0;
        virtual R visitNumberExpr(const Number &expr) = 0;
        virtual R visitUnaryExpr(const Unary &expr) = 0;
        virtual R visitVariableExpr(const Variable &expr) = 0;

        virtual ~Visitor() = default;
    };

    template <typename R> R accept(Visitor<R> &visitor) {
        return acceptImpl(visitor);
    }

  private:
    template <typename R> R acceptImpl(Visitor<R> &visitor) const;
};

struct Binary : Expr {
    std::shared_ptr<Expr> left;
    Token op;
    std::shared_ptr<Expr> right;

    Binary(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right)
        : left(left), op(op), right(right) {}

    template <typename R> R acceptImpl(Visitor<R> &visitor) const {
        return visitor.visitBinaryExpr(*this);
    }
};

struct Unary : Expr {
    Token op;
    std::shared_ptr<Expr> right;

    Unary(Token op, std::shared_ptr<Expr> right) : op(op), right(right) {}

    template <typename R> R acceptImpl(Visitor<R> &visitor) const {
        return visitor.visitUnaryExpr(*this);
    }
};

struct Number : Expr {
    Token value;

    Number(Token value) : value(value) {}

    template <typename R> R acceptImpl(Visitor<R> &visitor) const {
        return visitor.visitNumberExpr(*this);
    }
};

struct Variable : Expr {
    Token name;

    Variable(Token name) : name(name) {}

    template <typename R> R acceptImpl(Visitor<R> &visitor) const {
        return visitor.visitVariableExpr(*this);
    }
};

struct Grouping : Expr {
    std::shared_ptr<Expr> expression;

    Grouping(std::shared_ptr<Expr> expression) : expression(expression) {}

    template <typename R> R acceptImpl(Visitor<R> &visitor) const {
        return visitor.visitGroupingExpr(*this);
    }
};

struct Call : Expr {
    std::shared_ptr<Expr> callee;
    std::vector<std::shared_ptr<Expr>> args;

    Call(std::shared_ptr<Expr> callee, std::vector<std::shared_ptr<Expr>> args)
        : callee(callee), args(args) {}

    template <typename R> R acceptImpl(Visitor<R> &visitor) const {
        return visitor.visitCallExpr(*this);
    }
};

#endif // EXPR_HPP
