#include "pql/lexer.h"
#include "catch.hpp"
#include "logging.h"

TEST_CASE("Lexer tokenisation") {
    std::string testQuery = "assign a; variable v;\n Select a such that Uses (a, v) pattern a (v, _)";

    Lexer lexer(testQuery);

    REQUIRE(lexer.getNextToken() == Token{"assign", TokenType::Identifier});
    REQUIRE(lexer.getNextToken() == Token{"a", TokenType::Identifier});
    REQUIRE(lexer.getNextToken().getText() == ";");

    REQUIRE(lexer.getNextToken() == Token{"variable", TokenType::Identifier});
    REQUIRE(lexer.getNextToken() == Token{"v", TokenType::Identifier});
    REQUIRE(lexer.getNextToken().getText() == ";");

    REQUIRE(lexer.getNextReservedToken() == Token{"Select", TokenType::Select});
    REQUIRE(lexer.getNextToken() == Token{"a", TokenType::Identifier});

    REQUIRE(lexer.getNextReservedToken() == Token{"such that", TokenType::SuchThat});
    REQUIRE(lexer.getNextReservedToken() == Token{"Uses", TokenType::Uses});

    REQUIRE(lexer.getNextToken() == Token{"(", TokenType::OpeningParan});
    REQUIRE(lexer.getNextToken() == Token{"a", TokenType::Identifier});
    REQUIRE(lexer.getNextToken() == Token{",", TokenType::Comma});
    REQUIRE(lexer.getNextToken() == Token{"v", TokenType::Identifier});
    REQUIRE(lexer.getNextToken() == Token{")", TokenType::ClosingParan});

    REQUIRE(lexer.getNextReservedToken() == Token{"pattern", TokenType::Pattern});
    REQUIRE(lexer.getNextToken() == Token{"a", TokenType::Identifier});

    REQUIRE(lexer.getNextToken() == Token{"(", TokenType::OpeningParan});
    REQUIRE(lexer.getNextToken() == Token{"v", TokenType::Identifier});
    REQUIRE(lexer.getNextToken() == Token{",", TokenType::Comma});
    REQUIRE(lexer.getNextToken() == Token{"_", TokenType::Underscore});
    REQUIRE(lexer.getNextToken() == Token{")", TokenType::ClosingParan});
}

TEST_CASE("Lexer eatWhitespace") {
    std::string string1 = "   ";
    std::string string2 = "\n";

    Lexer lexer1(string1);
    lexer1.eatWhitespace();
    REQUIRE(lexer1.text.length() == 0);

    Lexer lexer2(string2);
    lexer2.eatWhitespace();
    REQUIRE(lexer2.text.length() == 0);
}

TEST_CASE("Lexer peekNextToken") {
    std::string testQuery = "assign a; \n Select a such that Uses (a, v) pattern a (v, _)";

    Lexer lexer(testQuery);

    REQUIRE(lexer.peekNextToken() == Token{"assign", TokenType::Identifier});
    REQUIRE(lexer.getNextToken() == Token{"assign", TokenType::Identifier});

    REQUIRE(lexer.getNextToken() == Token{"a", TokenType::Identifier});
    REQUIRE(lexer.getNextToken().getText() == ";");

    REQUIRE(lexer.peekNextReservedToken() == Token{"Select", TokenType::Select});
    REQUIRE(lexer.getNextReservedToken() == Token{"Select", TokenType::Select});
}