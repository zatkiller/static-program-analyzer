#include <string>

#include "lexer.h"

struct Parser {
    Lexer lexer;

    void checkType(Token, TokenType);
    Token getNextToken();
    Token peekNextToken();
    Token getAndCheckNextToken(TokenType);
    Token peekAndCheckNextToken(TokenType);

    void parsePql(std::string);
    void parseDeclarations();
};