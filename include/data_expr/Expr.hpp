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
struct Trinary;

struct Expr {
    virtual ~Expr() = default;

    struct Visitor {
        virtual void visitTrinaryExpr(const Trinary &expr) = 0;
        virtual void visitBinaryExpr(const Binary &expr) = 0;
        virtual void visitCallExpr(const Call &expr) = 0;
        virtual void visitGroupingExpr(const Grouping &expr) = 0;
        virtual void visitNumberExpr(const Number &expr) = 0;
        virtual void visitUnaryExpr(const Unary &expr) = 0;
        virtual void visitVariableExpr(const Variable &expr) = 0;

        virtual ~Visitor() = default;
    };

    virtual void accept(Visitor &visitor) = 0;
};

struct Trinary : Expr {
    std::shared_ptr<Expr> left;
    Token op1;
    std::shared_ptr<Expr> middle;
    Token op2;
    std::shared_ptr<Expr> right;

    explicit Trinary(std::shared_ptr<Expr> left, Token op1,
                     std::shared_ptr<Expr> middle, Token op2,
                     std::shared_ptr<Expr> right);

    void accept(Visitor &visitor) override;
};

struct Binary : Expr {
    std::shared_ptr<Expr> left;
    Token op;
    std::shared_ptr<Expr> right;

    explicit Binary(std::shared_ptr<Expr> left, Token op,
                    std::shared_ptr<Expr> right);

    void accept(Visitor &visitor) override;
};

struct Unary : Expr {
    Token op;
    std::shared_ptr<Expr> right;

    explicit Unary(Token op, std::shared_ptr<Expr> right);

    void accept(Visitor &visitor) override;
};

struct Number : Expr {
    Token value;

    explicit Number(Token value);

    void accept(Visitor &visitor) override;
};

struct Variable : Expr {
    Token name;

    explicit Variable(Token name);

    void accept(Visitor &visitor) override;
};

struct Grouping : Expr {
    std::shared_ptr<Expr> expression;

    explicit Grouping(std::shared_ptr<Expr> expression);

    void accept(Visitor &visitor) override;
};

struct Call : Expr {
    Token callee;
    std::vector<std::shared_ptr<Expr>> args;

    explicit Call(Token callee, std::vector<std::shared_ptr<Expr>> args);

    void accept(Visitor &visitor) override;
};

#endif // EXPR_HPP
