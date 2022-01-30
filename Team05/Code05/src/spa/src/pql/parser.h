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

    std::string getParsedText();

    void addPql(std::string);
    Query parsePql(std::string);

    void parseDeclarations(Query&);
    void parseDeclaration(Query&, DesignEntity);
};