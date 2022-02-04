#include <sstream>

#include "parser.h"
#include "logging.h"

#define LOGGER Logger(Level::DEBUG) << "parser.cpp: \n"

std::string Parser::getParsedText() {
    return lexer.text;
}

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

Token Parser::getAndCheckNextReservedToken(TokenType tokenType) {
    Token token = getNextReservedToken();
    checkType(token, tokenType);
    return token;
}

Token Parser::peekAndCheckNextReservedToken(TokenType tokenType) {
    Token token = peekNextReservedToken();
    checkType(token, tokenType);
    return token;
}

void Parser::parseDeclaration(Query &queryObj, DesignEntity de) {
    Token variable = getAndCheckNextToken(TokenType::Identifier);
    queryObj.addDeclaration(variable.getText(), de);
}

void Parser::parseDeclarations(Query &queryObj) {
    Token token = getAndCheckNextToken(TokenType::Identifier);
    auto iterator = designEntityMap.find(token.getText());
    if (iterator == designEntityMap.end())
        throw "No such design entity!";

    DesignEntity designEntity = iterator->second;
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
}

void Parser::parseSelectFields(Query &queryObj) {
    getAndCheckNextReservedToken(TokenType::Select);

    Token t = getAndCheckNextToken(TokenType::Identifier);
    std::string name = t.getText();

    if (!queryObj.hasDeclaration(name))
        throw "Variable does not exist in declaration!";

    queryObj.addVariable(name);

    Token nextToken = peekNextToken();

    // Handle multiple select clause, might be needed for future iterations?;
    while (nextToken.getTokenType() == TokenType::Comma) {
        t = getAndCheckNextToken(TokenType::Identifier);
        name = t.getText();
        if (!queryObj.hasDeclaration(name))
            throw "Variable does not exist in declaration!";

        queryObj.addVariable(name);
        nextToken = peekNextToken();
    }
}

bool Parser::isStmtRef(Token token, Query &queryObj) {
    TokenType type = token.getTokenType();
     if (type == TokenType::Number || type == TokenType::Underscore)
         return true;

     if (type == TokenType::Identifier && queryObj.hasDeclaration(token.getText()))
         return true;

     return false;
}

bool Parser::isEntRef(Token token, Query &queryObj) {
    TokenType type = token.getTokenType();
    if (type == TokenType::String || type == TokenType::Underscore)
        return true;

    if (type == TokenType::Identifier && queryObj.hasDeclaration(token.getText()))
        return true;

    return false;
}

StmtRef Parser::parseStmtRef(Query &queryObj) {
    Token token = getNextToken();
    TokenType type = token.getTokenType();
    StmtRef stmtRef;
    if (type == TokenType::Number) {
        int lineNo;
        std::stringstream ss(token.getText());
        ss >> lineNo;
        stmtRef = StmtRef::ofLineNo(lineNo);
    } else if (type == TokenType::Underscore) {
        stmtRef = StmtRef::ofWildcard();
    } else if (type == TokenType::Identifier) {
        stmtRef = StmtRef::ofDeclaration(token.getText());
    }

    return stmtRef;
}

EntRef Parser::parseEntRef(Query &queryObj) {
    Token token = getNextToken();
    TokenType type = token.getTokenType();
    EntRef entRef;
    if (type == TokenType::String) {
        entRef = EntRef::ofVarName(token.getText());
    } else if (type == TokenType::Underscore) {
        entRef = EntRef::ofWildcard();
    } else if (type == TokenType::Identifier) {
        entRef = EntRef::ofDeclaration(token.getText());
    }

    return entRef;
}

std::shared_ptr<RelRef> Parser::parseUses(Query &queryObj) {
    return parseRelRefVariables<Uses>(queryObj, &Uses::useStmt, &Uses::used);
}

std::shared_ptr<RelRef> Parser::parseModifies(Query &queryObj) {
    return parseRelRefVariables<Modifies>(queryObj, &Modifies::modifiesStmt, &Modifies::modified);
}

std::shared_ptr<RelRef> Parser::parseRelRef(Query &queryObj) {
    TokenType tokenType = getNextReservedToken().getTokenType();

    if (tokenType == TokenType::Uses) {
        return parseUses(queryObj);
    } else if (tokenType == TokenType::Modifies) {
        return parseModifies(queryObj);
    } else {
        throw "Don't recognize this relRef";
    }
}

std::string Parser::parseExpSpec() {
    std::string prefix = "", suffix = "", value = "";

    if (peekNextToken().getTokenType() == TokenType::Underscore) {
        getNextToken();
        prefix = "_";
    }

    if (peekNextToken().getTokenType() == TokenType::String) {
        Token t = getNextToken();
        value = t.getText();
    }

    if (peekNextToken().getTokenType() == TokenType::Underscore) {
        getNextToken();
        suffix = "_";
    }
    return prefix + value + suffix;
}

void Parser::parseSuchThat(Query &queryObj) {
    getAndCheckNextReservedToken(TokenType::SuchThat);
    std::shared_ptr<RelRef> relRef = parseRelRef(queryObj);
    queryObj.addSuchthat(relRef);
}
void Parser::parsePattern(Query &queryObj) {
    getAndCheckNextReservedToken(TokenType::Pattern);

    Token t = getAndCheckNextToken(TokenType::Identifier);
    std::string declarationName = t.getText();

    if (queryObj.getDeclarationDesignEntity(declarationName) != DesignEntity::ASSIGN)
        throw "Not an assignment!";

    Pattern p;

    p.synonym = declarationName;
    getAndCheckNextToken(TokenType::OpeningParan);
    p.lhs = parseEntRef(queryObj);
    getAndCheckNextToken(TokenType::Comma);
    p.expression = parseExpSpec();
    getAndCheckNextToken(TokenType::ClosingParan);

    queryObj.addPattern(p);
}


void Parser::parseQuery(Query &queryObj) {
    parseSelectFields(queryObj);

    if (peekNextReservedToken().getTokenType() == TokenType::SuchThat)
        parseSuchThat(queryObj);

    if (peekNextReservedToken().getTokenType() == TokenType::Pattern)
        parsePattern(queryObj);
}

Query Parser::parsePql(std::string query) {
    addPql(query);
    Query queryObj;

    for (Token token = peekNextReservedToken(); token.getTokenType() != TokenType::Eof; token = peekNextReservedToken()) {
        if (token.getTokenType() != TokenType::Select) {
            // Parse delcarations first
            parseDeclarations(queryObj);
        } else {
            // Start parsing the actual query
            parseQuery(queryObj);
        }
    }

    return queryObj;
};