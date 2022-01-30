#include "pql/lexer.h"
#include "catch.hpp"
#include "logging.h"

TEST_CASE("Test PQL Lexer") {
    std::string testQuery = "assign a; variable v;\n Select a such that Uses (a, v) pattern a (v, _)";

    Lexer lexer(testQuery);

    REQUIRE(lexer.getNextToken() == Token { "assign", TokenType::Identifier});
    REQUIRE(lexer.getNextToken() == Token { "a", TokenType::Identifier});
    REQUIRE(lexer.getNextToken().getText() == ";");

    REQUIRE(lexer.getNextToken() == Token { "variable", TokenType::Identifier});
    REQUIRE(lexer.getNextToken() == Token { "v", TokenType::Identifier});
    REQUIRE(lexer.getNextToken().getText() == ";");

    REQUIRE(lexer.getNextReservedToken() == Token { "Select", TokenType::Select});
}