#pragma once

#include <string>
#include <unordered_set>

#include "exceptions.h"
#include "QPS/Query.h"
#include "QPS/Lexer.h"

namespace qps::parser {

using qps::query::Query;
using qps::query::Declaration;
using qps::query::DesignEntity;
using qps::query::Elem;
using qps::query::RelRef;
using qps::query::EntRef;
using qps::query::StmtRef;
using qps::query::ExpSpec;
using qps::query::ModifiesP;
using qps::query::ModifiesS;
using qps::query::UsesP;
using qps::query::UsesS;
using qps::query::AttrName;
using qps::query::AttrRef;
using qps::query::AttrCompareRef;
using qps::query::Pattern;

/**
 * Struct used to represent the Parser
 * Handles the syntax and semantic checks of PQL
 */
struct Parser {
    Lexer lexer = Lexer("");

    /**
     * Checks if the token is of a specific type, if not, it throws an error
     * @param Token to check
     * @param type expected token type
     */
    static void checkType(Token &token, TokenType type);

    /**
     * Checks if the design entity can possess an AttrName. If not, it throws an erorr
     *
     * @param de the Design Entity
     * @param an the AttrName
     */
    static void checkDesignEntityAndAttrNameMatch(DesignEntity de, AttrName an);

    /**
     * Adds the specified query to parser
     *
     * @param query the query to add
     */
    void addInput(std::string_view query);

    /**
     * Returns a Query object representing the parsed query
     *
     * @param query the query to parse
     */
    Query parsePql(std::string_view query);

    /**
     * Parses the declarations of a query and adds them to
     * the query object
     *
     * @param query the query object
     */
    void parseDeclarationStmts(Query &queryObj);
    
    /**
     * Returns an element
     *
     * @param query the query object
     * @return the parsed eleme
     */
    Elem parseElem(Query &query);

    /**
     * Returns a tuple
     *
     * @param query the query object
     */
    std::vector<Elem> parseTuple(Query &query);

    /**
     * Parses the select fields of a QPS query into a ResultCl
     * and adds them to the query object
     *
     * @param query the query object
     */
    void parseSelectFields(Query &query);

    /**
     * Parses the such that clauses andall following such that clauses
     * adds them to the query ADT
     *
     * @param query the query ADT
     */
    void parseSuchThatClause(Query &query);

    /**
     * Parses a RelRef and adds them to a query object
     *
     * @param query the query object
     */
    void parseRelRef(Query &query);

    /**
     * Returns a shared pointer containg a pointer of type T, representing the relationship being parsed
     *
     * @param query the query object
     * @param f1 the memory address of the first RelRef variable
     * @param f2 the memory address of the second RelRef variable
     * @return shared pointer of type t
     */
    template<typename T, typename F1, typename F2>
    std::shared_ptr<T> parseRelRefVariables(Query &query, F1 T::* const f1, F2 T::* const f2) {
        // Template functions to be defined in header file
        // https://stackoverflow.com/questions/3040480/c-template-function-compiles-in-header-but-not-implementation
        std::shared_ptr<T> ptr = std::make_shared<T>();

        if constexpr (std::is_same_v<F1, StmtRef>) {
            ptr.get()->*f1 = parseStmtRef(query);
        } else {
            ptr.get()->*f1 = parseEntRef(query);
        }

        ptr.get()->validateFirstArg();

        getAndCheckNextToken(TokenType::COMMA);

        if constexpr(std::is_same_v<F2, StmtRef>) {
            ptr.get()->*f2 = parseStmtRef(query);
        } else {
            ptr.get()->*f2 = parseEntRef(query);
        }

        ptr.get()->validateSecondArg();

        return ptr;
    }

    /**
     * Returns a shared pointer containing a pointer of type RelRef, representing the Modifies
     * or Uses relationship parsed
     *
     * @param query the query object
     * @return shared pointer of type RelRef
     */
    std::shared_ptr<RelRef> parseModifiesOrUsesVariables(Query &query, TokenType type);

    /**
     * Returns a StmtRef that has been parsed
     *
     * @param query the query object
     * @return StmtRef that has been parsed
     */
    StmtRef parseStmtRef(Query &query);

    /**
     * Returns a EntRef that has been parsed
     *
     * @param query the query object
     * @return EntRef that has been parsed
     */
    EntRef parseEntRef(Query &query);

    /**
     * Returns a Pattern after parsing the Pattern variables
     *
     * @param query the query object
     * @param the declaration of the pattern
     * @return Pattern representing the parseed pattern
     */
    Pattern parsePatternVariables(Query &query, const Declaration& d);

    /**
     * Parses a Pattern and adds them to a query object
     *
     * @param query the query object
     */
    void parsePattern(Query &query);

    /**
     * Parses a Pattern and all following pattern clauses 
     * adds them to a query object
     *
     * @param query the query object
     */
    void parsePatternClause(Query &query);

    /**
     * Returns after parsing an AttrName
     *
     * @param query the query ADT
     * @param declaration the declaration the attrName belongs to
     * @return AttrName the attrname
     */
    AttrName parseAttrName(Query &query, const Declaration& declaration);

    /**
     * Parses and returns a AttrRef
     *
     * @param query the query ADT
     */
    AttrRef parseAttrRef(Query &query);

    /**
     * Parses and returns a AttrCompareRef
     *
     * @param query the query ADT
     */
    AttrCompareRef parseAttrCompareRef(Query &query);

    /**
     * Parses a with clause and all following with clauses
     * adds them to the query ADT
     *
     * @param query the query ADT
     */
    void parseWithClause(Query &query);

    /**
     * Parses attrCompare for the with clause
     *
     * @param query the query ADT
     */
    void parseAttrCompare(Query& query);

    /**
     * Throws an error if 2 AttrCompareRef cannot be compared
     */
    void validateComparingTypes(const AttrCompareRef& lhs, const AttrCompareRef& rhs);

    /**
     * Returns a ExpSpec belonging to an assign pattern
     */
    ExpSpec parseExpSpec();

private:
    /**
     * Returns a Declaration
     *
     * @param query the query object
     * @return the parsed declaration
     */
    Declaration parseDeclaration(Query& query);

    /**
     * Parses the declarations belonging to a single design entity
     * and adds them to the query object
     *
     * @param query the query object
     * @param entity the design entity to assign the declaration
     */
    void parseDeclarationStmt(Query& queryObj, DesignEntity de);

    /**
     * Checks whether a synonym wrapped in StmtRef has the correct Design Entity.
     *
     * @param query the query object
     * @param s the StmtRef
     * @return a bool value indicates whether a synonym is declared as a statement reference.
     */
    static bool isValidStatementType(Query& query, const StmtRef& s);

    /**
     * Checks whether a synonym wrapped in EntRef has the correct Design Entity.
     *
     * @param query the query object
     * @param e the EntRef
     * @return a bool value indicates whether a synonym is declared as a entity reference.
     */
    static bool isValidEntityType(Query& query, const EntRef& e);

    /**
     * Helper function to parse RelRef depending on the RelRef subclass
     *
     * @param query the query object
     */
    std::shared_ptr<RelRef> parseRelRefHelper(Query& query);

    /**
     * Returns a EntRef that has been parsed as the LHS of a pattern clause
     *
     * @param query the query object
     * @return EntRef representing the LHS of a pattern that has been parsed
     */
    EntRef parsePatternLhs(Query& query);

    /**
      * Parses the query section of the QPS query
      * and adds them to the query object
      *
      * @param query the query object
      */
    void parseQuery(Query& query);

    /**
     * Returns the next token from the lexified query
     *
     * @return a token
     */
    Token getNextToken();

    /**
     * Returns the next token of the query without modifying
     * the lexified query
     *
     * @return a token
     */
    Token peekNextToken();

    /**
     * Returns the next token that belongs to a reserved word
     *
     * @return a token
     */
    Token getNextReservedToken();

    /**
     * Returns the next token that belongs to a reserved word
     * without modifying the lexified queryy
     *
     * @return a token
     */
    Token peekNextReservedToken();

    /**
     * Returns the next token of the query without modifying
     * the lexified query while checking the type of the token
     *
     * @param type the token type to check for
     * @return a token
     */
    Token getAndCheckNextToken(TokenType type);

    /**
     * Returns the next token of the query without modifying
     * the lexified query while checking the type of the token
     *
     * @param type the token type to check for
     * @return a token
     */
    Token peekAndCheckNextToken(TokenType type);

    /**
     * Returns the next token that belongs to a reserved word
     * while checking the token type
     *
     * @param type the token type to check for
     * @return a token
     */
    Token getAndCheckNextReservedToken(TokenType type);

    /**
     * Returns the next token that belongs to a reserved word
     * without modifying the lexified query and checks the token type
     *
     * @param type the token type to check for
     * @return a token
     */
    Token peekAndCheckNextReservedToken(TokenType type);

    /**
     * returns true if 2 AttrCompareRef can be compared
     *
     * @param lhs the first attrcompareref to be compared
     * @param rhs the second attrcompareref to be compared
     * @return boolean result of true if they can be compared
     */
    bool isAttrCompareRefsComparable(const AttrCompareRef& lhs, const AttrCompareRef& rhs) const;
};

}  // namespace qps::parser
