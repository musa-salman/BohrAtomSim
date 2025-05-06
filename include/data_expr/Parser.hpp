#ifndef PARSER_HPP
#define PARSER_HPP

#include <memory>
#include <vector>

#include "Expr.hpp"
#include "data_expr/Token.hpp"

class Parser {

    std::shared_ptr<Expr> parseExpression();

    std::shared_ptr<Expr> parseEquality();

    std::shared_ptr<Expr> parseComparison();

    std::shared_ptr<Expr> parseTerm();

    std::shared_ptr<Expr> parseFactor();

    std::shared_ptr<Expr> parsePower();

    std::shared_ptr<Expr> parseUnary();

    std::shared_ptr<Expr> parsePrimary();

    std::shared_ptr<Expr> parseArguments();

    bool match(std::initializer_list<TokenType> types);

    bool check(TokenType type);

    Token advance();

    bool isAtEnd();

    Token peek();

    Token previous();

    void consume(TokenType type, const char *message) {
        if (check(type)) {
            advance();
            return;
        }
        throw ParserError(message);
    }

    std::vector<Token> tokens;
    size_t current;

  public:
    class ParserError : public std::exception {
        const char *message;

      public:
        ParserError(const char *message) : message(message) {}

        const char *what() const noexcept override { return message; }
    };

    Parser(const std::vector<Token> &tokens) : tokens(tokens), current(0) {}
};

#endif // PARSER_HPP