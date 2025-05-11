#include <string>

#include "Expr.hpp"

class AstPrinter : public Expr::Visitor {
    std::string result;

  public:
    std::string print(const std::shared_ptr<Expr> &expr) {
        expr->accept(*this);
        return result;
    }

    void visitTrinaryExpr(const Trinary &expr) override {
        parenthesize(expr.op1.lexeme, std::vector<std::shared_ptr<Expr>>{
                                          expr.left, expr.middle, expr.right});
    }

    void visitBinaryExpr(const Binary &expr) override {
        parenthesize(expr.op.lexeme, expr.left, expr.right);
    }

    void visitUnaryExpr(const Unary &expr) override {
        parenthesize(expr.op.lexeme, expr.right);
    }

    void visitGroupingExpr(const Grouping &expr) override {
        parenthesize("group", expr.expression);
    }

    void visitNumberExpr(const Number &expr) override {
        result += std::to_string(std::any_cast<double>(expr.value.literal));
    }

    void visitVariableExpr(const Variable &expr) override {
        result += expr.name.lexeme;
    }

    void visitCallExpr(const Call &expr) override {
        parenthesize("call " + expr.callee.lexeme, expr.args);
    }

  private:
    void parenthesize(const std::string &name,
                      const std::shared_ptr<Expr> &expr) {
        parenthesize(name, std::vector<std::shared_ptr<Expr>>{expr});
    }

    void parenthesize(const std::string &name,
                      const std::vector<std::shared_ptr<Expr>> &exprs) {
        result += "(" + name;
        for (const auto &expr : exprs) {
            result += " ";
            expr->accept(*this);
        }

        result += ")";
    }

    void parenthesize(const std::string &name,
                      const std::shared_ptr<Expr> &left,
                      const std::shared_ptr<Expr> &right) {
        parenthesize(name, std::vector<std::shared_ptr<Expr>>{left, right});
    }
};