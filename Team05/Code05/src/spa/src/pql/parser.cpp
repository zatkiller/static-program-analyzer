#include "parser.h"

#include "query.h"

Token Parser::getNextToken() {
    return lexer.getNextToken();
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

void Parser::parseDeclarations() {
    Token token = getAndCheckNextToken(TokenType::Identifier);
    auto iterator = designEntityMap.find(token.getText());
    if (iterator == designEntityMap.end())
        throw "No such design entity!";

    DESIGN_ENTITY designEntity = iterator->second;


}

void Parser::parsePql(std::string query) {
    parseDeclarations();
};