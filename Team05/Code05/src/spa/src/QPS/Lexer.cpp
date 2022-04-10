#include <unordered_map>
#include <vector>

#include "messages.h"
#include "exceptions.h"
#include "QPS/Lexer.h"

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
            { '%', TokenType::MODULO },
            { '=', TokenType::EQUAL },
            { '<', TokenType::LEFT_ARROW_HEAD },
            { '>', TokenType::RIGHT_ARROW_HEAD },
    };

    std::vector<std::string> keywords = {
            "Select", "BOOLEAN", "Modifies", "Uses", "Parent*",
            "Parent", "Follows*", "Follows", "Next*",
            "Next", "Calls*", "Calls", "Affects*", "Affects",
            "pattern", "such that", "with", "procName", "varName",
            "value", "stmt#", "and"
    };

    std::unordered_map<std::string, TokenType> keywordsToTokenTypeMap {
            { "Select", TokenType::SELECT },
            { "BOOLEAN", TokenType::BOOLEAN },
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
            { "Affects*", TokenType::AFFECTS_T },
            { "Affects", TokenType::AFFECTS },
            { "and", TokenType::AND },
            { "pattern", TokenType::PATTERN },
            { "such that", TokenType::SUCH_THAT },
            { "with", TokenType::WITH },
            { "procName", TokenType::PROCNAME },
            { "varName", TokenType::VARNAME },
            { "value", TokenType::VALUE },
            { "stmt#", TokenType::STMTNUM }
    };

    Token Lexer::getReservedToken(std::string keyword) {
        auto pos = keywordsToTokenTypeMap.find(keyword);
        if (pos == keywordsToTokenTypeMap.end()) {
            throw exceptions::PqlSyntaxException(messages::qps::parser::keywordDoesNotExist);
        }
        return Token { keyword, pos->second };
    }

    std::string_view Lexer::getText() {
        return text;
    }

    void Lexer::eatWhitespace() {
        while (text.length() > 0 && (isspace(text[0]) || text[0] == '\n' || text[0] == '\t'))
            text.remove_prefix(1);
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
        std::string strValue = std::string { text.substr(pos1 + 1, num_chars - 1) };

        text.remove_prefix(num_chars + 1);

        return Token { strValue, TokenType::STRING };
    }

    Token Lexer::getIdentifier() {
        int charCount = 0;

        while (text.length() > charCount && (isalpha(text[charCount]) || isdigit(text[charCount])))
            charCount++;

        std::string identifier = std::string { text.substr(0, charCount) };
        text.remove_prefix(charCount);

        return Token { identifier, TokenType::IDENTIFIER };
    }

    Token Lexer::getNumber() {
        int charCount = 0;

        while (text.length() > charCount && (isdigit(text[charCount])))
            charCount++;

        std::string number = std::string { text.substr(0, charCount) };

        if (number[0] == '0' && number.length() > 1)
            throw exceptions::PqlSyntaxException(messages::qps::parser::leadingZeroMessage);

        text.remove_prefix(charCount);

        return Token { number, TokenType::NUMBER };
    }

    Token Lexer::getSpecialChar() {
        Token token;
        std::string value = std::string { text.substr(0, 1) };
        TokenType type = getSpecialCharTokenType(text[0]);

        if (type == TokenType::INVALID) {
            token = Token { "", type };
        } else {
            token = Token { value, type };
        }

        text.remove_prefix(1);
        return token;
    }

    Token Lexer::getNextToken() {
        eatWhitespace();

        if (text.length() == 0) {
            return Token{ "EOF", TokenType::END_OF_FILE };
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
            token = Token { "EOF", TokenType::END_OF_FILE };
            return token;
        }

        for (auto keyword : keywords) {
            if (hasPrefix(keyword)) {
                token = getReservedToken(keyword);
                break;
            }
        }

        text.remove_prefix(token.getText().length());
        return token;
    }

    Token Lexer::peekNextToken() {
        std::string_view copy = text;
        Token token = getNextToken();
        text = copy;
        return token;
    }

    Token Lexer::peekNextReservedToken() {
        std::string_view copy = text;
        Token token = getNextReservedToken();
        text = copy;
        return token;
    }

    bool Lexer::hasLeadingWhitespace() {
        return isspace(text[0]);
    }
}  // namespace qps::parser

