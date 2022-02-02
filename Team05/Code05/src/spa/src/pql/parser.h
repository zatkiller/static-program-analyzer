#pragma once

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
    Token getAndCheckNextReservedToken(TokenType);
    Token peekAndCheckNextReservedToken(TokenType);

    std::string getParsedText();

    void addPql(std::string);
    Query parsePql(std::string);

    void parseDeclarations(Query&);
    void parseDeclaration(Query&, DesignEntity);
    void parseQuery(Query&);
    void parseSelectFields(Query&);
    void parseSuchThat(Query&);
    std::shared_ptr<RelRef> parseRelRef(Query&);

    template<typename T, typename F1, typename F2>
    std::shared_ptr<T> parseRelRefVariables(Query &queryObj, F1 f1, F2 f2) {
        std::shared_ptr<T> ptr = std::make_shared<T>();
        getAndCheckNextToken(TokenType::OpeningParan);

        if (!isStmtRef(peekNextToken(), queryObj))
            throw "Not a valid StmtRef!";

        ptr.get()->*f1 = parseStmtRef(queryObj);
        getAndCheckNextToken(TokenType::Comma);

        if (!isEntRef(peekNextToken(), queryObj))
            throw "Not a valid EntRef!";

        ptr.get()->*f2 = parseEntRef(queryObj);
        getAndCheckNextToken(TokenType::ClosingParan);
        return ptr;
    };

    std::shared_ptr<RelRef> parseUses(Query &);
    std::shared_ptr<RelRef> parseModifies(Query &);

    bool isStmtRef(Token, Query &);
    bool isEntRef(Token, Query &);
    StmtRef parseStmtRef(Query &);
    EntRef parseEntRef(Query &);
};