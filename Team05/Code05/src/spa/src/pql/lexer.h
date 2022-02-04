#pragma once

#include <string>
#include <vector>

enum class TokenType {
    END_OF_FILE,
    INVALID,

    UNDERSCORE,
    SEMICOLON,
    COMMA,
    PERIOD,
    OPENING_PARAN,
    CLOSING_PARAN,

    IDENTIFIER,
    NUMBER,
    STRING,

    // Relationships
    USES,
    MODIFIES,

    // Reserved Keywords
    SELECT,
    SUCH_THAT,
    PATTERN
};

struct Token {
    std::string text;
    TokenType type;

    TokenType getTokenType() {
        return this->type;
    }

    std::string getText() {
        return this->text;
    }

    bool operator==(const Token &o) const {
        return (this->type == o.type) && (this->text == o.text);
    }
};

struct Lexer {
    std::string text;

    Lexer(std::string input) : text(input) {};

    void eatWhitespace();

    bool hasPrefix(std::string prefix);

    std::string getText();

    Token getNextToken();
    Token getNextReservedToken();
    Token peekNextToken();
    Token peekNextReservedToken();

    bool operator==(const Lexer &o) const {
        return this->text == o.text;
    }
};


