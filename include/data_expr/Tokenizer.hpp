#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include <unordered_map>
#include <vector>

#include "Token.hpp"

class Tokenizer {
  public:
    Tokenizer(const std::string_view &expression) : expression(expression) {
        current = 0;
        start = 0;
        tokens.reserve(expression.length() / 2);
    }

    std::vector<Token> scanTokens();

  private:
    static const std::unordered_map<std::string_view, TokenType> keywords;

    const std::string_view expression;

    void scanToken();

    char peak() const;
    char peakNext() const;
    char advance();
    bool match(char expected);
    bool isAtEnd() const;

    void addToken(TokenType type, std::any literal = {});

    void tokenizeNumber();
    void tokenizeIdentifier();

    void skipWhitespace();

    size_t current;
    size_t start;
    std::vector<Token> tokens;
};

#endif // TOKENIZER_HPP