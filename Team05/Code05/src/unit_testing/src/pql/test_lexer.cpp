#include "pql/lexer.h"
#include "catch.hpp"
#include "logging.h"

TEST_CASE("Lexer tokenisation") {
    std::string testQuery = "assign a; variable v;\n Select a such that Uses (a, v) pattern a (v, _)";

    Lexer lexer(testQuery);

    REQUIRE(lexer.getNextToken() == Token{"assign", TokenType::IDENTIFIER});
    REQUIRE(lexer.getNextToken() == Token{"a", TokenType::IDENTIFIER});
    REQUIRE(lexer.getNextToken().getText() == ";");

    REQUIRE(lexer.getNextToken() == Token{"variable", TokenType::IDENTIFIER});
    REQUIRE(lexer.getNextToken() == Token{"v", TokenType::IDENTIFIER});
    REQUIRE(lexer.getNextToken().getText() == ";");

    REQUIRE(lexer.getNextReservedToken() == Token{"Select", TokenType::SELECT});
    REQUIRE(lexer.getNextToken() == Token{"a", TokenType::IDENTIFIER});

    REQUIRE(lexer.getNextReservedToken() == Token{"such that", TokenType::SUCH_THAT});
    REQUIRE(lexer.getNextReservedToken() == Token{"Uses", TokenType::USES});

    REQUIRE(lexer.getNextToken() == Token{"(", TokenType::OPENING_PARAN});
    REQUIRE(lexer.getNextToken() == Token{"a", TokenType::IDENTIFIER});
    REQUIRE(lexer.getNextToken() == Token{",", TokenType::COMMA});
    REQUIRE(lexer.getNextToken() == Token{"v", TokenType::IDENTIFIER});
    REQUIRE(lexer.getNextToken() == Token{")", TokenType::CLOSING_PARAN});

    REQUIRE(lexer.getNextReservedToken() == Token{"pattern", TokenType::PATTERN});
    REQUIRE(lexer.getNextToken() == Token{"a", TokenType::IDENTIFIER});

    REQUIRE(lexer.getNextToken() == Token{"(", TokenType::OPENING_PARAN});
    REQUIRE(lexer.getNextToken() == Token{"v", TokenType::IDENTIFIER});
    REQUIRE(lexer.getNextToken() == Token{",", TokenType::COMMA});
    REQUIRE(lexer.getNextToken() == Token{"_", TokenType::UNDERSCORE});
    REQUIRE(lexer.getNextToken() == Token{")", TokenType::CLOSING_PARAN});
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

    REQUIRE(lexer.peekNextToken() == Token{"assign", TokenType::IDENTIFIER});
    REQUIRE(lexer.getNextToken() == Token{"assign", TokenType::IDENTIFIER});

    REQUIRE(lexer.getNextToken() == Token{"a", TokenType::IDENTIFIER});
    REQUIRE(lexer.getNextToken().getText() == ";");

    REQUIRE(lexer.peekNextReservedToken() == Token{"Select", TokenType::SELECT});
    REQUIRE(lexer.getNextReservedToken() == Token{"Select", TokenType::SELECT});
}


TEST_CASE("Lexer getNexToken where next token is a string") {
    std::string testQuery = "\"x\"";
    Lexer lexer(testQuery);

    Token token = lexer.getNextToken();
    REQUIRE(token.getTokenType() == TokenType::STRING);
    REQUIRE(token.getText() == "x");

    lexer.text="_\"x\"_";
    REQUIRE(lexer.getNextToken().getTokenType() == TokenType::UNDERSCORE);
    token = lexer.getNextToken();
    REQUIRE(token.getTokenType() == TokenType::STRING);
    REQUIRE(token.getText() == "x");
    REQUIRE(lexer.getNextToken().getTokenType() == TokenType::UNDERSCORE);
}