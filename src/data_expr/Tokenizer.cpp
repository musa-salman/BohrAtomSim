
#include <cctype>
#include <string>

#include "data_expr/Tokenizer.hpp"

std::vector<Token> Tokenizer::scanTokens() {
    if (isAtEnd())
        return tokens;

    while (!isAtEnd()) {
        start = current;
        scanToken();
    }

    addToken(TokenType::END);
    return tokens;
}

void Tokenizer::scanToken() {
    skipWhitespace();
    if (isAtEnd())
        return;

    char c = advance();
    switch (c) {
    case '(':
        addToken(TokenType::LEFT_PAREN);
        break;
    case ')':
        addToken(TokenType::RIGHT_PAREN);
        break;
    case ',':
        addToken(TokenType::COMMA);
        break;
    case '-':
        addToken(TokenType::MINUS);
        break;
    case '+':
        addToken(TokenType::PLUS);
        break;
    case '*':
        addToken(TokenType::STAR);
        break;
    case '/':
        addToken(TokenType::SLASH);
        break;
    case '^':
        addToken(TokenType::CARET);
        break;
    case '!':
        addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
        break;
    case '=':
        addToken(TokenType::EQUAL);
        break;
    case '>':
        addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
        break;
    case '<':
        addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
        break;
    default:
        if (isdigit(c))
            tokenizeNumber();
        else if (isalpha(c))
            tokenizeIdentifier();
        else {
            addToken(TokenType::ERROR);
            break;
        }
    }
}

void Tokenizer::skipWhitespace() {
    while (!isAtEnd() && isspace(peak()))
        advance();
}

char Tokenizer::peak() const {
    if (isAtEnd())
        return '\0';

    return expression[current];
}

char Tokenizer::peakNext() const {
    if (current + 1 >= expression.length())
        return '\0';

    return expression[current + 1];
}

char Tokenizer::advance() { return expression[current++]; }

bool Tokenizer::isAtEnd() const { return current >= expression.length(); }

bool Tokenizer::match(char expected) {
    if (isAtEnd() || expression[current] != expected)
        return false;

    current++;
    return true;
}

void Tokenizer::tokenizeNumber() {
    while (isdigit(peak()))
        advance();

    if (peak() == '.' && isdigit(peakNext())) {
        advance();
        while (isdigit(peak()))
            advance();
    }

    if (peak() == 'e' || peak() == 'E') {
        advance();
        if (peak() == '+' || peak() == '-')
            advance();

        while (isdigit(peak()))
            advance();
    }

    addToken(TokenType::NUMBER,
             std::stod(std::string(expression.substr(start, current - start))));
}

void Tokenizer::tokenizeIdentifier() {
    size_t start = current;
    while (isalnum(peak()) || peak() == '_')
        advance();

    std::string_view lexeme = expression.substr(start, current - start);
    auto it = keywords.find(lexeme);
    if (it != keywords.end()) {
        addToken(it->second);
        return;
    }

    addToken(TokenType::IDENTIFIER);
}

void Tokenizer::addToken(TokenType type, std::any literal) {
    tokens.emplace_back(
        type, std::string(expression.substr(start, current - start)), literal);
}
