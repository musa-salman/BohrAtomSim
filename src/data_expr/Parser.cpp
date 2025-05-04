#include "data_expr/Parser.hpp"
#include "data_expr/Expr.hpp"
#include "data_expr/Token.hpp"
#include <cstdarg>

/*
    expression -> equality
    equality -> comparison ( ( "!=" | "=" ) comparison )*
    comparison -> term ( ( ">" | ">=" | "<" | "<=" ) term )*
    term -> factor ( ( "-" | "+" ) factor )*
    factor -> power ( ( "/" | "*" ) power )*
    power -> unary ( "^" unary )?
    unary -> ( "-" | "!" ) unary | primary

    primary -> IDENTIFIER "(" arguments ")" | IDENTIFIER | NUMBER | "("
   expression ")"

    arguments -> expression ( "," expression )*
*/

std::shared_ptr<Expr> Parser::parseExpression() { return parseEquality(); }

std::shared_ptr<Expr> Parser::parseEquality() {
    auto expr = parseComparison();

    while (match(TokenType::BANG_EQUAL, TokenType::EQUAL)) {
        auto op = previous();
        auto right = parseComparison();
        expr = std::make_shared<Binary>(expr, op, right);
    }

    return expr;
}

std::shared_ptr<Expr> Parser::parseComparison() {
    auto expr = parseTerm();

    while (match(TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS,
                 TokenType::LESS_EQUAL)) {
        auto op = previous();
        auto right = parseTerm();
        expr = std::make_shared<Binary>(expr, op, right);
    }

    return expr;
}

std::shared_ptr<Expr> Parser::parseTerm() {
    auto expr = parseFactor();

    while (match(TokenType::MINUS, TokenType::PLUS)) {
        auto op = previous();
        auto right = parseFactor();
        expr = std::make_shared<Binary>(expr, op, right);
    }

    return expr;
}

std::shared_ptr<Expr> Parser::parseFactor() {
    auto expr = parsePower();

    while (match(TokenType::SLASH, TokenType::STAR)) {
        auto op = previous();
        auto right = parsePower();
        expr = std::make_shared<Binary>(expr, op, right);
    }

    return expr;
}

std::shared_ptr<Expr> Parser::parsePower() {
    auto expr = parseUnary();

    while (match(TokenType::CARET)) {
        auto op = previous();
        auto right = parseUnary();
        expr = std::make_shared<Binary>(expr, op, right);
    }

    return expr;
}

std::shared_ptr<Expr> Parser::parseUnary() {
    if (match(TokenType::BANG, TokenType::MINUS)) {
        auto op = previous();
        auto right = parseUnary();
        return std::make_shared<Unary>(op, right);
    }

    return parsePrimary();
}

std::shared_ptr<Expr> Parser::parsePrimary() {
    if (match(TokenType::NUMBER)) {
        return std::make_shared<Number>(previous());
    }

    if (match(TokenType::IDENTIFIER)) {
        Token name = previous();

        if (match(TokenType::LEFT_PAREN)) {
            std::vector<std::shared_ptr<Expr>> args;

            if (!check(TokenType::RIGHT_PAREN)) {
                do {
                    args.push_back(parseExpression());
                } while (match(TokenType::COMMA));
            }

            consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");
            return std::make_shared<Call>(name, args);
        }

        return std::make_shared<Variable>(name);
    }

    if (match(TokenType::LEFT_PAREN)) {
        auto expr = parseExpression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_shared<Grouping>(expr);
    }

    throw ParserError("Expect expression.");
}

bool Parser::match(TokenType types...) {
    va_list args;
    va_start(args, types);
    for (TokenType type = types; type != TokenType::END;
         type = va_arg(args, TokenType)) {
        if (check(type)) {
            advance();
            va_end(args);
            return true;
        }
    }
    va_end(args);
    return false;
}

bool Parser::check(TokenType type) {
    if (isAtEnd())
        return false;
    return peek().type == type;
}

Token Parser::advance() {
    if (!isAtEnd())
        current++;
    return previous();
}

bool Parser::isAtEnd() { return peek().type == TokenType::END; }

Token Parser::peek() { return tokens[current]; }

Token Parser::previous() { return tokens[current - 1]; }
