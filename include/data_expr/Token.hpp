#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <any>
#include <string>
#include <unordered_map>

enum class TokenType {
    LEFT_PAREN,
    RIGHT_PAREN,
    COMMA,
    MINUS,
    PLUS,
    STAR,
    SLASH,
    CARET,
    COLON,

    BANG,
    BANG_EQUAL,
    EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,
    APPROX,

    IDENTIFIER,
    NUMBER,

    PI,
    E,

    AND,
    OR,

    ERROR,
    END
};

struct Token {
    const TokenType type;
    const std::string lexeme;
    const std::any literal;

    Token(TokenType type, std::string lexeme, std::any literal)
        : type(type), lexeme(std::move(lexeme)), literal(literal) {}
};

class TokenUtils {
  public:
    static const std::unordered_map<std::string_view, TokenType> &keywords() {
        static const std::unordered_map<std::string_view, TokenType>
            keywordTokenMap = {{"and", TokenType::AND},
                               {"or", TokenType::OR},
                               {"pi", TokenType::PI},
                               {"e", TokenType::E}};
        return keywordTokenMap;
    }
};

#endif // TOKEN_HPP
