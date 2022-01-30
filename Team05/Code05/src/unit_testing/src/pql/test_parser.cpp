#include "pql/parser.h"
#include "catch.hpp"

TEST_CASE("Test Parser peekNext and getNext") {
    std::string testQuery = "assign a; \n Select a such that Uses (a, v) pattern a (v, _)";

    Parser parser;
    parser.addPql(testQuery);

    REQUIRE(parser.peekNextToken() == Token{"assign", TokenType::Identifier});
    REQUIRE(parser.peekAndCheckNextToken(TokenType::Identifier) == Token{"assign", TokenType::Identifier});

    REQUIRE(parser.getNextToken() == Token{"assign", TokenType::Identifier});
    REQUIRE(parser.getAndCheckNextToken(TokenType::Identifier) == Token{"a", TokenType::Identifier});
}

TEST_CASE("Test Parser addPql on generating new Lexer") {
    std::string testQuery1 = "assign a; \n Select a such that Uses (a, v) pattern a (v, _)";
    std::string testQuery2 = "variable v; \n Select v such that Uses (a, v) pattern a (v, _)";

    Parser parser;
    parser.addPql(testQuery1);

    Lexer lexer1 = parser.lexer;
    REQUIRE(parser.getParsedText() == testQuery1);

    parser.addPql(testQuery2);

    REQUIRE(parser.getParsedText() == testQuery2);

    REQUIRE(!(lexer1 == parser.lexer));
}

TEST_CASE("Test Parser parseDeclarations method on single Design Entity") {
    std::string testQuery = "assign a, a1; \n Select a such that Uses (a, v) pattern a (v, _)";

    Parser parser;
    parser.addPql(testQuery);

    Query queryObj;

    parser.parseDeclarations(queryObj);

    REQUIRE(queryObj.declarations.count("a") > 0);
    REQUIRE(queryObj.declarations.find("a")->second == DESIGN_ENTITY::ASSIGN);

    REQUIRE(queryObj.declarations.count("a1") > 0);
    REQUIRE(queryObj.declarations.find("a1")->second == DESIGN_ENTITY::ASSIGN);
}

TEST_CASE("Test Parser parseDeclarations method on multiple Design Entity") {
    std::string testQuery = "assign a; variable v, v1;\n Select a such that Uses (a, v) pattern a (v, _)";

    Parser parser;
    parser.addPql(testQuery);

    Query queryObj;

    parser.parseDeclarations(queryObj);

    REQUIRE(queryObj.declarations.count("a") > 0);
    REQUIRE(queryObj.declarations.find("a")->second == DESIGN_ENTITY::ASSIGN);

    parser.parseDeclarations(queryObj);

    REQUIRE(queryObj.declarations.count("v") > 0);
    REQUIRE(queryObj.declarations.find("v")->second == DESIGN_ENTITY::VARIABLE);

    REQUIRE(queryObj.declarations.count("v1") > 0);
    REQUIRE(queryObj.declarations.find("v1")->second == DESIGN_ENTITY::VARIABLE);
}