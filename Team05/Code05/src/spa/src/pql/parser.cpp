#include "parser.h"

Token Parser::getNextToken() {
    return lexer.getNextToken();
}
Token Parser::peekNextToken() {
    return lexer.peekNextToken();
}

Token Parser::getNextReservedToken() {
    return lexer.getNextReservedToken();
}
Token Parser::peekNextReservedToken() {
    return lexer.peekNextReservedToken();
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

    while (peekNextToken().getTokenType() == TokenType::Comma) {
        token = getAndCheckNextToken(TokenType::Comma);
        parseDeclaration(queryObj, designEntity);
    }

    getAndCheckNextToken(TokenType::Semicolon);
}

void Parser::addPql(std::string query) {
    lexer = Lexer(query);
};

void Parser::parsePql(std::string query) {
    addPql(query);
    Query queryObj;

    for (Token token = peekNextReservedToken(); token.getTokenType() != TokenType::Eof; token = peekNextReservedToken()) {
        if (token.getTokenType() == TokenType::Select) {
            // Parse query here
        } else {
            parseDeclarations(queryObj);
        }
    }
};