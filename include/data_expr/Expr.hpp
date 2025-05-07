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

    template <typename R> R accept(Visitor<R> &visitor);

  private:
    template <typename R> R acceptImpl(Visitor<R> &visitor) const;
};

struct Binary : Expr {
    std::shared_ptr<Expr> left;
    Token op;
    std::shared_ptr<Expr> right;

    explicit Binary(std::shared_ptr<Expr> left, Token op,
                    std::shared_ptr<Expr> right);

    template <typename R> R acceptImpl(Visitor<R> &visitor) const;
};

struct Unary : Expr {
    Token op;
    std::shared_ptr<Expr> right;

    explicit Unary(Token op, std::shared_ptr<Expr> right);

    template <typename R> R acceptImpl(Visitor<R> &visitor) const;
};

struct Number : Expr {
    Token value;

    explicit Number(Token value) : value(value) {}

    template <typename R> R acceptImpl(Visitor<R> &visitor) const;
};

struct Variable : Expr {
    Token name;

    explicit Variable(Token name);

    template <typename R> R acceptImpl(Visitor<R> &visitor) const;
};

struct Grouping : Expr {
    std::shared_ptr<Expr> expression;

    explicit Grouping(std::shared_ptr<Expr> expression);

    template <typename R> R acceptImpl(Visitor<R> &visitor) const;
};

struct Call : Expr {
    Token callee;
    std::vector<std::shared_ptr<Expr>> args;

    explicit Call(Token callee, std::vector<std::shared_ptr<Expr>> args);

    template <typename R> R acceptImpl(Visitor<R> &visitor) const;
};

#endif // EXPR_HPP
