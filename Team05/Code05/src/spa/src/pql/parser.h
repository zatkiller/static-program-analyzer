#pragma once

#include <string>

#include "exceptions.h"
#include "pql/query.h"
#include "pql/lexer.h"

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

    // Template functions to be defined in header file
    // https://stackoverflow.com/questions/3040480/c-template-function-compiles-in-header-but-not-implementation
    template<typename T, typename F1, typename F2>
    std::shared_ptr<T> parseRelRefVariables(Query &queryObj, F1 f1, F2 f2) {
        std::shared_ptr<T> ptr = std::make_shared<T>();
        getAndCheckNextToken(TokenType::OPENING_PARAN);

        if (!isStmtRef(peekNextToken(), queryObj))
            throw exceptions::PqlSyntaxException(messages::qps::parser::invalidStmtRefMessage);

        ptr.get()->*f1 = parseStmtRef(queryObj);
        getAndCheckNextToken(TokenType::COMMA);

        if (!isStmtRef(peekNextToken(), queryObj))
            throw exceptions::PqlSyntaxException(messages::qps::parser::invalidStmtRefMessage);

        ptr.get()->*f2 = parseStmtRef(queryObj);
        getAndCheckNextToken(TokenType::CLOSING_PARAN);
        return ptr;
    }

    std::shared_ptr<RelRef> parseModifiesOrUsesVariables(Query &, TokenType);

    bool isStmtRef(Token, Query &);
    bool isEntRef(Token, Query &);
    StmtRef parseStmtRef(Query &);
    EntRef parseEntRef(Query &);

    void parsePattern(Query&);
    std::string parseExpSpec();
};