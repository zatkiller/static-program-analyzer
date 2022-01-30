#include "parser.h"

Token Parser::getNextToken() {
    return lexer.getNextToken();
}
Token Parser::peekNextToken() {
    return lexer.peekNextToken();
}


void Parser::checkType(Token token, TokenType tokenType) {
    if (token.getTokenType() != tokenType)
        throw "Not expecting this token type!";
}

Token Parser::getAndCheckNextToken(TokenType tt) {
    Token token = getNextToken();
    checkType(token, tt);
    return token;
}

Token Parser::peekAndCheckNextToken(TokenType tt) {
    Token token = peekNextToken();
    checkType(token, tt);
    return token;
}

void Parser::parseDeclaration(Query &queryObj, DESIGN_ENTITY de) {
    Token variable = getAndCheckNextToken(TokenType::Identifier);
    queryObj.addDeclaration(variable.getText(), de);
}

void Parser::parseDeclarations(Query &queryObj) {
    Token token = getAndCheckNextToken(TokenType::Identifier);
    auto iterator = designEntityMap.find(token.getText());
    if (iterator == designEntityMap.end())
        throw "No such design entity!";

    DESIGN_ENTITY designEntity = iterator->second;
    parseDeclaration(queryObj, designEntity);
    // Parse and add single declaration to Query Object
}

void Parser::parsePql(std::string query) {
    Query queryObj;
    parseDeclarations(queryObj);
};