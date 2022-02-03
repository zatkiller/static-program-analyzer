#pragma once

#include <string>
#include <vector>
#include <queue>
#include <iostream>
#include <variant>

enum class TokenType {
    eof,
    name,     // [a-zA-Z][a-zA-Z0-9]*
    number,   // [0-9]+
    special,  // special characters like +-*/><=;
};

struct Token {
    TokenType type;
    std::variant<char, std::string, int> value;
    bool operator==(const Token& o) const {
        return type == o.type && value == o.value;
    }
    Token& operator=(const Token& other) {
        this->type = other.type;
        this->value = other.value;
        return *this;
    }
};


class Lexer {
private:
    std::deque<Token> tokens;
    void lex(const std::string& source);
public:
    explicit Lexer(const std::string& source) {
        this->lex(source);
    }
    std::deque<Token>& getTokens();
};
