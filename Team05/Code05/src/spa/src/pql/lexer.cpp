#include <stdio.h>

#include "logging.h"
#include "pql/lexer.h"

#define LOGGER Logger(Level::DEBUG) << "pql/lexer.cpp: \n"

std::string Lexer::getText() {
    return text;
}

void Lexer::eatWhitespace() {
    while (text.length() > 0 && (isspace(text[0]) || text[0] == '\n' || text[0] == '\t'))
        text.erase(0, 1);
}

bool Lexer::hasPrefix(std::string prefix) {
    return text.find(prefix) == 0;
}

Token Lexer::getNextToken() {
    eatWhitespace();

    if (text.length() == 0) {
        return Token{"EOF", TokenType::END_OF_FILE};
    } else if (text[0] == '"') {
        auto pos1 = text.find("\"");
        auto pos2 = text.find("\"", pos1 + 1);
        int num_chars = pos2 - pos1;
        std::string strValue = text.substr(pos1 + 1, num_chars - 1);

        text.erase(0, num_chars + 1);

        return Token { strValue, TokenType::STRING };
    } else if (isalpha(text[0])) {
        int charCount = 0;

        while (text.length() > 0 && (isalpha(text[charCount]) || isdigit(text[charCount])))
            charCount++;

        std::string identifier = text.substr(0, charCount);
        text.erase(0, charCount);

        return Token { identifier, TokenType::IDENTIFIER};
    } else if (isdigit(text[0])) {
        int charCount = 0;
        while (text.length() > 0 && (isdigit(text[charCount])))
            charCount++;

        std::string number = text.substr(0, charCount);
        text.erase(0, charCount);

        return Token { number, TokenType::NUMBER};
    } else {
            Token token;
            std::string value = text.substr(0, 1);
            if (text[0] == ';') {
                token = {value, TokenType::SEMICOLON};
            } else if (text[0] == '(') {
                token = {value, TokenType::OPENING_PARAN};
            } else if (text[0] == ')') {
                token = {value, TokenType::CLOSING_PARAN};
            } else if (text[0] == '_') {
                token = {value, TokenType::UNDERSCORE};
            } else if (text[0] == ',') {
                token = {value, TokenType::COMMA};
            } else {
                token = {"", TokenType::INVALID};
            }

            text.erase(0, 1);
            return token;
        }
}

Token Lexer::getNextReservedToken() {
    eatWhitespace();

    Token token;
    if (text.length() == 0) {
        token = Token{"EOF", TokenType::END_OF_FILE};
    } else if (hasPrefix("Select")) {
        token = Token {"Select", TokenType::SELECT};
    } else if (hasPrefix("Modifies")) {
        token =  Token {"Modifies", TokenType::MODIFIES};
    } else if (hasPrefix("Uses")) {
        token = Token {"Uses", TokenType::USES};
    } else if (hasPrefix("pattern")) {
        token =  Token {"pattern", TokenType::PATTERN};
    } else if (hasPrefix("such that")) {
        token =  Token {"such that", TokenType::SUCH_THAT};
    } else {
        token = Token {"", TokenType::INVALID};
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
