
#include "Expr.hpp"

class AstPrinter : public Expr::Visitor<std::string> {
  public:
    std::string print(const std::shared_ptr<Expr> &expr) {
        return expr->accept(*this);
    }

    std::string visitBinaryExpr(const Binary &expr) override {
        return parenthesize(expr.op.lexeme, expr.left, expr.right);
    }

    std::string visitUnaryExpr(const Unary &expr) override {
        return parenthesize(expr.op.lexeme, expr.right);
    }

    std::string visitGroupingExpr(const Grouping &expr) override {
        return parenthesize("group", expr.expression);
    }

    std::string visitNumberExpr(const Number &expr) override {
        return std::to_string(std::any_cast<double>(expr.value.literal));
    }

    std::string visitVariableExpr(const Variable &expr) override {
        return std::string(expr.name.lexeme);
    }

    std::string visitCallExpr(const Call &expr) override {
        std::string result = parenthesize("call", expr.callee);
        for (const auto &arg : expr.args) {
            result += " " + arg->accept(*this);
        }
        return result;
    }

  private:
    std::string parenthesize(const std::string &name,
                             const std::shared_ptr<Expr> &expr) {
        return parenthesize(name, std::vector<std::shared_ptr<Expr>>{expr});
    }

    std::string parenthesize(const std::string &name,
                             const std::vector<std::shared_ptr<Expr>> &exprs) {
        std::string result = "(" + name;
        for (const auto &expr : exprs)
            result += " " + expr->accept(*this);

        result += ")";
        return result;
    }

    std::string parenthesize(const std::string &name,
                             const std::shared_ptr<Expr> &left,
                             const std::shared_ptr<Expr> &right) {
        return parenthesize(name,
                            std::vector<std::shared_ptr<Expr>>{left, right});
    }
};