#include <string>

#include "query.h"
#include "lexer.h"

struct Parser {
    Lexer lexer = Lexer("");

    void checkType(Token, TokenType);
    Token getNextToken();
    Token peekNextToken();
    Token getNextReservedToken();
    Token peekNextReservedToken();
    Token getAndCheckNextToken(TokenType);
    Token peekAndCheckNextToken(TokenType);

    void addPql(std::string);
    void parsePql(std::string);
    void parseDeclarations(Query&);
    void parseDeclaration(Query&, DESIGN_ENTITY);
};