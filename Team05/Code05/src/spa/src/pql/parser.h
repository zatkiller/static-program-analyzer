#pragma once

#include <string>
#include <unordered_set>

#include "exceptions.h"
#include "pql/query.h"
#include "pql/lexer.h"

namespace qps::parser {

using qps::query::Query;
using qps::query::DesignEntity;
using qps::query::RelRef;
using qps::query::EntRef;
using qps::query::StmtRef;
using qps::query::ExpSpec;

/**
 * Struct used to represent the Parser
 * Handles the syntax and semantic checks of PQL
 */
struct Parser {
    Lexer lexer = Lexer("");

    void checkType(Token, TokenType);

    /*
     * Returns the next token from the lexified query
     *
     * @return a token
     */
    Token getNextToken();

    /*
     * Returns the next token of the query without modifying
     * the lexified query
     *
     * @return a token
     */
    Token peekNextToken();

    /*
     * Returns the next token that belongs to a reserved word
     *
     * @return a token
     */
    Token getNextReservedToken();

    /*
     * Returns the next token that belongs to a reserved word
     * without modifying the lexified queryy
     *
     * @return a token
     */
    Token peekNextReservedToken();

    /*
     * Returns the next token of the query without modifying
     * the lexified query while checking the type of the token
     *
     * @param type the token type to check for
     * @return a token
     */
    Token getAndCheckNextToken(TokenType type);

    /*
     * Returns the next token of the query without modifying
     * the lexified query while checking the type of the token
     *
     * @param type the token type to check for
     * @return a token
     */
    Token peekAndCheckNextToken(TokenType type);

    /*
     * Returns the next token that belongs to a reserved word
     * while checking the token type
     *
     * @param type the token type to check for
     * @return a token
     */
    Token getAndCheckNextReservedToken(TokenType type);

    /*
     * Returns the next token that belongs to a reserved word
     * without modifying the lexified query and checks the token type
     *
     * @param type the token type to check for
     * @return a token
     */
    Token peekAndCheckNextReservedToken(TokenType type);

    /*
     * Returns the curent state of parsed query
     *
     * @return the current state of the parsed string
     */
    std::string getParsedText();


    /*
     * Adds the specified query to parser
     *
     * @param query the query to add
     */
    void addInput(std::string query);

    /*
     * Returns a Query object representing the parsed query
     *
     * @param query the query to parse
     */
    Query parsePql(std::string query);

    /*
     * Parses the declarations of a query and adds them to
     * the query object
     *
     * @param query the query object
     */
    void parseDeclarations(Query &query);

    /*
     * Parses the declarations belonging to a single design entity
     * and adds them to the query object
     *
     * @param query the query object
     * @param entity the design entity to assign the declaration
     */
    void parseDeclaration(Query &query, DesignEntity entity);

    /*
     * Parses the query section of the pql query
     * and adds them to the query object
     *
     * @param query the query object
     */
    void parseQuery(Query &query);

    /*
     * Parses the select fields of a pql query
     * and adds them to the query object
     *
     * @param query the query object
     */
    void parseSelectFields(Query &query);

    /*
     * Parses the such that section of the pql query
     * and adds them to the query object
     *
     * @param query the query object
     */
    void parseSuchThat(Query &query);

    /*
     * Parses a RelRef and adds them to a query object
     *
     * @param query the query object
     */
    std::shared_ptr<RelRef> parseRelRef(Query &query);


    /**
     * Checks whether a synonym wrapped in StmtRef has the correct Design Entity.
     *
     * @param query the query object
     * @param s the StmtRef
     * @return a bool value indicates whether a synonym is declared as a statement reference.
     */
    bool isValidStatementType(Query &query, StmtRef s);

    /*
     * Returns a shared pointer containg a pointer of type T, representing the relationship being parsed
     *
     * @param query the query object
     * @param f1 the memory address of the first RelRef variable
     * @param f2 the memory address of the second RelRef variable
     * @return shared pointer of type t
     */
    template<typename T, typename F1, typename F2>
    std::shared_ptr<T> parseRelRefVariables(Query &query, F1 f1, F2 f2) {
        // Template functions to be defined in header file
        // https://stackoverflow.com/questions/3040480/c-template-function-compiles-in-header-but-not-implementation
        std::shared_ptr<T> ptr = std::make_shared<T>();
        getAndCheckNextToken(TokenType::OPENING_PARAN);

        if (!isStmtRef(peekNextToken(), query))
            throw exceptions::PqlSyntaxException(messages::qps::parser::invalidStmtRefMessage);

        StmtRef s1 = parseStmtRef(query);

        if (!isValidStatementType(query, s1))
            throw exceptions::PqlSemanticException(messages::qps::parser::notStatementSynonymMessage);

        ptr.get()->*f1 = s1;
        getAndCheckNextToken(TokenType::COMMA);

        if (!isStmtRef(peekNextToken(), query))
            throw exceptions::PqlSyntaxException(messages::qps::parser::invalidStmtRefMessage);


        StmtRef s2 = parseStmtRef(query);

        if (!isValidStatementType(query, s2))
            throw exceptions::PqlSemanticException(messages::qps::parser::notStatementSynonymMessage);

        ptr.get()->*f2 = s2;
        getAndCheckNextToken(TokenType::CLOSING_PARAN);
        return ptr;
    }

    /*
     * Returns a shared pointer containing a pointer of type RelRef, representing the Modifies
     * or Uses relationship parsed
     *
     * @param query the query object
     * @return shared pointer of type RelRef
     */
    std::shared_ptr<RelRef> parseModifiesOrUsesVariables(Query &query, TokenType type);

    bool isStmtRef(Token, Query &);

    bool isEntRef(Token, Query &);

    /*
     * Returns a StmtRef that has been parsed
     *
     * @param query the query object
     * @return StmtRef that has been parsed
     */
    StmtRef parseStmtRef(Query &query);

    /*
     * Returns a EntRef that has been parsed
     *
     * @param query the query object
     * @return EntRef that has been parsed
     */
    EntRef parseEntRef(Query &query);

    /*
     * Parses a Pattern and adds them to a query object
     *
     * @param query the query object
     */
    void parsePattern(Query &query);
    ExpSpec parseExpSpec();

    /*
     * Validate the expression in Pattern using Pratt parsing, which is an enhancement of recursive descent algorithm
     * but with precedence and associativity
     *
     * @param expr expression under pattern to be parsed
     */
    void validateExpr(std::string expr);

    /*
     * The base function to start the Pratt parsing
     */
    void parseExpr();

    /*
     * Parses the current expression
     */
    void parseCurrentExpr();

    /*
     * Parses the next expression. Expressions can be considered different if they have different operator precedence
     *
     * @param priority - priority the next expression holds
     *
     */
    void parseNextExpr(int priority);

    /*
     * get priority of operator token
     *
     * @param token token to get priority
     */
    int getOperatorPriority(Token token);
};

}  // namespace qps::parser
