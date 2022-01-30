#include <string>

#include "lexer.h"

struct Parser {
    Lexer lexer;

    Token getNextToken();
    void parsePql(std::string);
    void parseDeclarations();
    void checkType(Token, TokenType);
    Token getAndCheckNextToken(TokenType);
};