#pragma once

#include <string>
#include <vector>
#include <queue>
#include <iostream>
#include <variant>

enum class TokenType {
    eof,
    name,    // [a-zA-Z][a-zA-Z0-9]*
    number,  // [0-9]+
    special, // special characters like +-*/><=;
};

struct Token {
    const TokenType type;
    const std::variant<char, std::string, int> value;
    bool operator==(const Token& o) const {
        return type == o.type && value == o.value;
    }
};


class Lexer {
private:
    std::queue<Token> tokens;
    void lex(std::string& source);
public:
    Lexer(std::string& source) {
        this->lex(source);
    }
    std::queue<Token>& getTokens();
};
