#pragma once

#include <string>
#include <vector>

namespace qps::parser {

enum class TokenType {
    END_OF_FILE,
    INVALID,

    UNDERSCORE,
    SEMICOLON,
    COMMA,
    OPENING_PARAN,
    CLOSING_PARAN,
    PERIOD,
    EQUAL,
    AND,
    LEFT_ARROW_HEAD,
    RIGHT_ARROW_HEAD,

    IDENTIFIER,
    NUMBER,
    STRING,

    // Relationships
    USES,
    MODIFIES,
    FOLLOWS,
    FOLLOWS_T,
    PARENT,
    PARENT_T,
    NEXT,
    NEXT_T,
    CALLS,
    CALLS_T,
    AFFECTS,
    AFFECTS_T,

    // Reserved Keywords
    SELECT,
    SUCH_THAT,
    PATTERN,
    WITH,

    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    MODULO,

    PROCNAME,
    VARNAME,
    VALUE,
    STMTNUM,

    BOOLEAN
};

/**
* Struct used to represent a Token
*/
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

/**
* Struct used to represent a Lexer, which is responsible for tokenizing the
* the PQL query input
*/
struct Lexer {
    std::string text;

    explicit Lexer(std::string input) : text(input) {}

    void eatWhitespace();

    /**
     * Returns a boolean if the curent lexified query
     * has the specified prefix
     *
     * @param prefix the prefix to check
     * @return a boolean
     */
    bool hasPrefix(std::string prefix);

    std::string getText();

    /**
     * Returns the next token from the lexified query
     *
     * @return a token
     */
    Token getNextToken();

    /**
     * Returns the next token that belongs to a reserved word
     *
     * @return a token
     */
    Token getNextReservedToken();

    /**
     * Returns the next token of the query without modifying
     * the lexified query
     *
     * @return a token
     */
    Token peekNextToken();

    /**
     * Returns the next token that belongs to a reserved word
     * without modifying the lexified queryy
     *
     * @return a token
     */
    Token peekNextReservedToken();

    TokenType getSpecialCharTokenType(char ch);

    Token getString();
    Token getIdentifier();
    Token getNumber();
    Token getSpecialChar();

    Token getReservedToken(std::string keyword);

    bool hasLeadingWhitespace();

    bool operator==(const Lexer &o) const {
        return this->text == o.text;
    }
};

}  // namespace qps::parser


