#include <stdio.h>
#include <unordered_map>
#include <vector>

#include "logging.h"
#include "exceptions.h"
#include "pql/lexer.h"

#define LOGGER Logger(Level::DEBUG) << "pql/lexer.cpp: \n"

namespace qps::parser {
    std::unordered_map<char, TokenType> specialCharToTokenTypeMap {
            { ';', TokenType::SEMICOLON },
            { '(', TokenType::OPENING_PARAN },
            { ')', TokenType::CLOSING_PARAN },
            { '_', TokenType::UNDERSCORE },
            { '.', TokenType::PERIOD },
            { ',', TokenType::COMMA },
            { '+', TokenType::PLUS },
            { '-', TokenType::MINUS },
            { '*', TokenType::MULTIPLY },
            { '/', TokenType::DIVIDE },
            { '%', TokenType::MODULO }
    };

    std::vector<std::string> keywords = {
            "Select", "Modifies", "Uses", "Parent*",
            "Parent", "Follows*", "Follows", "Next*",
            "Next", "Calls*", "Calls", "pattern", "such that",
            "with"
    };

    std::unordered_map<std::string, TokenType> keywordsToTokenTypeMap {
            { "Select", TokenType::SELECT },
            { "Modifies", TokenType::MODIFIES },
            { "Uses", TokenType::USES },
            { "Parent*", TokenType::PARENT_T },
            { "Parent", TokenType::PARENT },
            { "Follows*", TokenType::FOLLOWS_T },
            { "Follows", TokenType::FOLLOWS },
            { "Next*", TokenType::NEXT_T },
            { "Next", TokenType::NEXT },
            { "Calls*", TokenType::CALLS_T },
            { "Calls", TokenType::CALLS },
            { "pattern", TokenType::PATTERN },
            { "such that", TokenType::SUCH_THAT },
            { "with", TokenType::WITH }
    };

    Token Lexer::getReservedToken(std::string keyword) {
        auto pos = keywordsToTokenTypeMap.find(keyword);
        if (pos == keywordsToTokenTypeMap.end()) {
            throw exceptions::PqlException("Keyword does not exist in map");
        }
        return Token { keyword, pos->second };
    }

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

    TokenType Lexer::getSpecialCharTokenType(char ch) {
        auto pos = specialCharToTokenTypeMap.find(ch);

        if (pos == specialCharToTokenTypeMap.end()) {
            return TokenType::INVALID;
        }

        return pos->second;
    }

    Token Lexer::getString() {
        auto pos1 = text.find("\"");
        auto pos2 = text.find("\"", pos1 + 1);
        int num_chars = pos2 - pos1;
        std::string strValue = text.substr(pos1 + 1, num_chars - 1);

        text.erase(0, num_chars + 1);

        return Token {strValue, TokenType::STRING };
    }

    Token Lexer::getIdentifier() {
        int charCount = 0;

        while (text.length() > 0 && (isalpha(text[charCount]) || isdigit(text[charCount])))
            charCount++;

        std::string identifier = text.substr(0, charCount);
        text.erase(0, charCount);

        return Token { identifier, TokenType::IDENTIFIER };
    }

    Token Lexer::getNumber() {
        int charCount = 0;
        while (text.length() > 0 && (isdigit(text[charCount])))
            charCount++;

        std::string number = text.substr(0, charCount);
        text.erase(0, charCount);

        return Token { number, TokenType::NUMBER };
    }

    Token Lexer::getSpecialChar() {
        Token token;
        std::string value = text.substr(0, 1);
        TokenType type = getSpecialCharTokenType(text[0]);

        if (type == TokenType::INVALID) {
            token = { "", type };
        } else {
            token = { value, type };
        }

        text.erase(0, 1);
        return token;
    }

    Token Lexer::getNextToken() {
        eatWhitespace();

        if (text.length() == 0) {
            return Token{"EOF", TokenType::END_OF_FILE};
        } else if (text[0] == '"') {
            return getString();
        } else if (isalpha(text[0])) {
            return getIdentifier();
        } else if (isdigit(text[0])) {
            return getNumber();
        } else {
            return getSpecialChar();
        }
    }

    Token Lexer::getNextReservedToken() {
        eatWhitespace();

        Token token = Token{ "", TokenType::INVALID };

        if (text.length() == 0) {
            token = Token {"EOF", TokenType::END_OF_FILE};
            return token;
        }

        for (auto keyword : keywords) {
            if (hasPrefix(keyword)) {
                token = getReservedToken(keyword);
                break;
            }
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
}  // namespace qps::parser

