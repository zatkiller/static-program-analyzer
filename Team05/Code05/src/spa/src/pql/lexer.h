#pragma once

#include <string>
#include <vector>
#include <string_view>

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

    TokenType getTokenType() const { return type; }
    std::string getText() const { return text; }

    bool operator==(const Token &o) const { return (type == o.type) && (text == o.text); }
};

/**
* Struct used to represent a Lexer, which is responsible for tokenizing the
* the PQL query input
*/
struct Lexer {
    std::string_view text;

    explicit Lexer(const std::string_view input) : text(input) {}

    /**
     * Removes all preceding whitespace in the lexer text
     */
    void eatWhitespace();

    /**
     * Returns a boolean if the curent lexified query
     * has the specified prefix
     *
     * @param prefix the prefix to check
     * @return a boolean
     */
    bool hasPrefix(std::string prefix);

    std::string_view getText();

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

    /*
    * returns true if the current PQL string has leading whitespace
    * @return boolean value if the lexer text has leading whiesapce
    */
    bool hasLeadingWhitespace();

    bool operator==(const Lexer &o) const { return text == o.text; }

private:
    /*
    * Returns tokentype of the special character
    * @param ch char of the special char
    * @return TokenType of the special char
    */
    TokenType getSpecialCharTokenType(char ch);

    /*
    * Returns Token representing the parsed String
    * @return Token representing the parsed String
    */
    Token getString();
    /*
    * Returns Token representing the parsed Identifier
    * @return Token representing the parsed Identifier
    */
    Token getIdentifier();
    /*
    * Returns Token representing the parsed Number
    * @return Token representing the parsed Number
    */
    Token getNumber();
    /*
    * Returns Token representing the parsed special char
    * @return Token representing the parsed special char
    */
    Token getSpecialChar();

    /*
    * Returns Token based on the provided keyword
    * @param keyword the reserved keyword
    * @return Token of the reserved keyword
    */
    Token getReservedToken(std::string keyword);
};

}  // namespace qps::parser


