#include "data_expr/Parser.hpp"
#include "data_expr/Expr.hpp"
#include "data_expr/Token.hpp"
#include <cstdarg>

/*
    expression -> logical_or
    logical_or -> logical_and ( "or" logical_and )*
    logical_and -> approx_equal ( "and" approx_equal )*

    approx_equal -> equality ( "~=" equality ( ":" equality )? )* ;
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

std::shared_ptr<Expr> Parser::parseExpression() { return parseLogicalOr(); }

std::shared_ptr<Expr> Parser::parseLogicalOr() {
    auto expr = parseLogicalAnd();

    while (match({TokenType::OR})) {
        auto op = previous();
        auto right = parseLogicalAnd();
        expr = std::make_shared<Binary>(expr, op, right);
    }

    return expr;
}

std::shared_ptr<Expr> Parser::parseLogicalAnd() {
    auto expr = parseApprox();
    while (match({TokenType::AND})) {
        auto op = previous();
        auto right = parseApprox();
        expr = std::make_shared<Binary>(expr, op, right);
    }
    return expr;
}

std::shared_ptr<Expr> Parser::parseApprox() {
    auto expr = parseEquality();

    while (match({TokenType::APPROX})) {
        auto op = previous();
        auto right = parseEquality();

        if (match({TokenType::COLON})) {
            auto op2 = previous();
            auto right2 = parseEquality();

            if (!std::dynamic_pointer_cast<Number>(right2))
                throw ParserError(
                    "Expect number after ':' in approx operator.");

            expr = std::make_shared<Trinary>(expr, op, right, op2, right2);
        } else
            expr = std::make_shared<Binary>(expr, op, right);
    }

    return expr;
}

std::shared_ptr<Expr> Parser::parseEquality() {
    auto expr = parseComparison();

    while (match({TokenType::BANG_EQUAL, TokenType::EQUAL})) {
        auto op = previous();
        auto right = parseComparison();
        expr = std::make_shared<Binary>(expr, op, right);
    }

    return expr;
}

std::shared_ptr<Expr> Parser::parseComparison() {
    auto expr = parseTerm();

    while (match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS,
                  TokenType::LESS_EQUAL})) {
        auto op = previous();
        auto right = parseTerm();
        expr = std::make_shared<Binary>(expr, op, right);
    }

    return expr;
}

std::shared_ptr<Expr> Parser::parseTerm() {
    auto expr = parseFactor();

    while (match({TokenType::MINUS, TokenType::PLUS})) {
        auto op = previous();
        auto right = parseFactor();
        expr = std::make_shared<Binary>(expr, op, right);
    }

    return expr;
}

std::shared_ptr<Expr> Parser::parseFactor() {
    auto expr = parsePower();

    while (match({TokenType::SLASH, TokenType::STAR})) {
        auto op = previous();
        auto right = parsePower();
        expr = std::make_shared<Binary>(expr, op, right);
    }

    return expr;
}

std::shared_ptr<Expr> Parser::parsePower() {
    auto expr = parseUnary();

    while (match({TokenType::CARET})) {
        auto op = previous();
        auto right = parseUnary();
        expr = std::make_shared<Binary>(expr, op, right);
    }

    return expr;
}

std::shared_ptr<Expr> Parser::parseUnary() {
    if (match({TokenType::BANG, TokenType::MINUS})) {
        auto op = previous();
        auto right = parseUnary();
        return std::make_shared<Unary>(op, right);
    }

    return parsePrimary();
}

std::shared_ptr<Expr> Parser::parsePrimary() {
    if (match({TokenType::NUMBER})) {
        return std::make_shared<Number>(previous());
    }

    if (match({TokenType::IDENTIFIER})) {
        Token name = previous();

        if (match({TokenType::LEFT_PAREN})) {
            std::vector<std::shared_ptr<Expr>> args;

            if (!check(TokenType::RIGHT_PAREN)) {
                do {
                    args.push_back(parseExpression());
                } while (match({TokenType::COMMA}));
            }

            consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");
            return std::make_shared<Call>(name, args);
        }

        return std::make_shared<Variable>(name);
    }

    if (match({TokenType::LEFT_PAREN})) {
        auto expr = parseExpression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_shared<Grouping>(expr);
    }

    throw ParserError("Expect expression.");
}

bool Parser::match(std::initializer_list<TokenType> types) {
    for (auto type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
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
