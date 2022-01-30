#include <string>
#include <vector>

enum class TokenType {
    Eof,
    Invalid,

    Underscore,
    Semicolon,
    Comma,
    Period,
    OpeningParan,
    ClosingParan,

    Identifier,
    Number,
    String,

    // Relationships
    Uses,
    Modifies,

    // Reserved Keywords
    Select,
    SuchThat,
    Pattern,
    Assign,

    //Relationship variables
    Value,
    StmtNum,
    VarName,
    ProcName
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

    Token getNextToken();
    Token getNextReservedToken();
    Token peekNextToken();
    Token peekNextReservedToken();
};


