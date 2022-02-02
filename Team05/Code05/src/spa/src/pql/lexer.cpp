#include <stdio.h>

#include "lexer.h"

std::string Lexer::getText() {
    return text;
}

void Lexer::eatWhitespace() {
    while (text.length() > 0 && (isspace(text[0])|| text[0] == '\n'))
        text.erase(0, 1);
}

bool Lexer::hasPrefix(std::string prefix) {
    return text.find(prefix) == 0;
}

Token Lexer::getNextToken() {
    eatWhitespace();

    if (text.length() == 0) {
        return Token{"", TokenType::Eof};
    } else if (text[0] == '"') {
        // Remove opening "
        text.erase(0, 1);

        int charCount = 0;
        while (text.length() > 0 && text[charCount] != '"')
            charCount++;

        std::string strValue = text.substr(charCount);

        // Remove closing "
        text.erase(0, 1);

        return Token { strValue, TokenType::String };
    } else if (isalpha(text[0])) {
        int charCount = 0;
        while (text.length() > 0 && (isalpha(text[charCount]) || isdigit(text[charCount])))
            charCount++;

        std::string identifier = text.substr(0, charCount);
        text.erase(0, charCount);

        return Token { identifier, TokenType::Identifier};
    } else if (isdigit(text[0])) {
        int charCount = 0;
        while (text.length() > 0 && (isdigit(text[charCount])))
            charCount++;

        std::string number = text.substr(charCount);
        text.erase(0, charCount);

        return Token { number, TokenType::Number};
    } else {
            Token token;
            std::string value = text.substr(0, 1);
            if (text[0] == ';') {
                token = {value, TokenType::Semicolon};
            } else if (text[0] == '(') {
                token = {value, TokenType::OpeningParan};
            } else if (text[0] == ')') {
                token = {value, TokenType::ClosingParan};
            } else if (text[0] == '_') {
                token = {value, TokenType::Underscore};
            } else if (text[0] == ',') {
                token = {value, TokenType::Comma};
            } else {
                token = {"", TokenType::Invalid};
            }

            text.erase(0, 1);
            return token;
        }
}

Token Lexer::getNextReservedToken() {
    eatWhitespace();

    Token token;
    if (hasPrefix("Select")) {
        token = Token {"Select", TokenType::Select};
    } else if (hasPrefix("Modifies")) {
        token =  Token {"Modifies", TokenType::Modifies};
    } else if (hasPrefix("Uses")) {
        token = Token {"Uses", TokenType::Uses};
    } else if (hasPrefix("pattern")) {
        token =  Token {"pattern", TokenType::Pattern};
    } else if (hasPrefix("such that")) {
        token =  Token {"such that", TokenType::SuchThat};
    } else {
        token = Token {"", TokenType::Invalid};
    }

    text.erase(0, token.getText().length());
    return token;
}

Token Lexer::peekNextToken() {
    std::string copy = text;
    Token token = getNextToken();
    text = copy;
    return token;
}

Token Lexer::peekNextReservedToken() {
    std::string copy = text;
    Token token = getNextReservedToken();
    text = copy;
    return token;
}
