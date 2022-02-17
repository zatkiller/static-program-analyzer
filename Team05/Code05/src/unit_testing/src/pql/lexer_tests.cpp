#include "pql/lexer.h"
#include "catch.hpp"

using qps::parser::Lexer;
using qps::parser::Token;
using qps::parser::TokenType;

TEST_CASE("Lexer getText") {
    std::string testQuery = "assign a; variable v;\n Select a such that Uses (a, v) pattern a (v, _)";
    Lexer lexer(testQuery);

    REQUIRE(lexer.getText() == testQuery);
}

TEST_CASE("Lexer eatWhitespace") {
    Lexer lexer1("   ");
    lexer1.eatWhitespace();
    REQUIRE(lexer1.text.length() == 0);

    Lexer lexer2("\n");
    lexer2.eatWhitespace();
    REQUIRE(lexer2.text.length() == 0);

    Lexer lexer3("\t");
    lexer3.eatWhitespace();
    REQUIRE(lexer3.text.length() == 0);
}

TEST_CASE("Lexer hasPrefx") {
    Lexer lexer("assign a;");
    REQUIRE(lexer.hasPrefix("assign"));
}

TEST_CASE("Lexer getNextToken") {
    Lexer lexer("123 hi123 hi \"test\" ;()_,.");

    auto t1 = lexer.getNextToken();
    REQUIRE(t1.getTokenType() == TokenType::NUMBER);
    REQUIRE(t1.getText() == "123");

    t1 = lexer.getNextToken();
    REQUIRE(t1.getTokenType() == TokenType::IDENTIFIER);
    REQUIRE(t1.getText() == "hi123");

    t1 = lexer.getNextToken();
    REQUIRE(t1.getTokenType() == TokenType::IDENTIFIER);
    REQUIRE(t1.getText() == "hi");

    t1 = lexer.getNextToken();
    REQUIRE(t1.getTokenType() == TokenType::STRING);
    REQUIRE(t1.getText() == "test");

    t1 = lexer.getNextToken();
    REQUIRE(t1.getTokenType() == TokenType::SEMICOLON);
    REQUIRE(t1.getText() == ";");

    t1 = lexer.getNextToken();
    REQUIRE(t1.getTokenType() == TokenType::OPENING_PARAN);
    REQUIRE(t1.getText() == "(");

    t1 = lexer.getNextToken();
    REQUIRE(t1.getTokenType() == TokenType::CLOSING_PARAN);
    REQUIRE(t1.getText() == ")");

    t1 = lexer.getNextToken();
    REQUIRE(t1.getTokenType() == TokenType::UNDERSCORE);
    REQUIRE(t1.getText() == "_");

    t1 = lexer.getNextToken();
    REQUIRE(t1.getTokenType() == TokenType::COMMA);
    REQUIRE(t1.getText() == ",");

    t1 = lexer.getNextToken();
    REQUIRE(t1.getTokenType() == TokenType::INVALID);
    REQUIRE(t1.getText() == "");

    t1 = lexer.getNextToken();
    REQUIRE(t1.getTokenType() == TokenType::END_OF_FILE);
}

TEST_CASE("Lexer peekNextToken") {
    Lexer lexer("hello 123 hello123");
    REQUIRE(lexer.peekNextToken() == Token{"hello", TokenType::IDENTIFIER});
    REQUIRE(lexer.getNextToken() == Token{"hello", TokenType::IDENTIFIER});
}

TEST_CASE("Lexer getNextReservedToken") {
    Lexer lexer("Select Modifies Uses Follows Follows* Parent Parent* pattern such that");

    auto t1 = lexer.getNextReservedToken();
    REQUIRE(t1.getTokenType() == TokenType::SELECT);
    REQUIRE(t1.getText() == "Select");

    t1 = lexer.getNextReservedToken();
    REQUIRE(t1.getTokenType() == TokenType::MODIFIES);
    REQUIRE(t1.getText() == "Modifies");

    t1 = lexer.getNextReservedToken();
    REQUIRE(t1.getTokenType() == TokenType::USES);
    REQUIRE(t1.getText() == "Uses");

    t1 = lexer.getNextReservedToken();
    REQUIRE(t1.getTokenType() == TokenType::FOLLOWS);
    REQUIRE(t1.getText() == "Follows");

    t1 = lexer.getNextReservedToken();
    REQUIRE(t1.getTokenType() == TokenType::FOLLOWS_T);
    REQUIRE(t1.getText() == "Follows*");

    t1 = lexer.getNextReservedToken();
    REQUIRE(t1.getTokenType() == TokenType::PARENT);
    REQUIRE(t1.getText() == "Parent");

    t1 = lexer.getNextReservedToken();
    REQUIRE(t1.getTokenType() == TokenType::PARENT_T);
    REQUIRE(t1.getText() == "Parent*");

    t1 = lexer.getNextReservedToken();
    REQUIRE(t1.getTokenType() == TokenType::PATTERN);
    REQUIRE(t1.getText() == "pattern");

    t1 = lexer.getNextReservedToken();
    REQUIRE(t1.getTokenType() == TokenType::SUCH_THAT);
    REQUIRE(t1.getText() == "such that");
}

TEST_CASE("Lexer peekNextReservedToken") {
    Lexer lexer("Select Modifies Uses");

    auto t1 = lexer.peekNextReservedToken();
    REQUIRE(t1.getTokenType() == TokenType::SELECT);
    REQUIRE(t1.getText() == "Select");

    t1 = lexer.getNextReservedToken();
    REQUIRE(t1.getTokenType() == TokenType::SELECT);
    REQUIRE(t1.getText() == "Select");
}

