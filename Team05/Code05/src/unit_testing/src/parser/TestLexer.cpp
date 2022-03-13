
#include "Parser/Lexer.h"
#include "logging.h"

#include "catch.hpp"

#define TEST_LOG Logger() << "TestLexer.cpp "

namespace sp {
namespace parser {

    /**
     * Ensures token structs and token type are correct.
     */
    TEST_CASE("Token testing") {
        Token name = { TokenType::name, "hello" };
        Token number = { TokenType::number, 1 };
        Token special = { TokenType::special, '+' };

        TEST_LOG << "Testing token value";
        // Value test
        REQUIRE(std::get<std::string>(name.value) == "hello");
        REQUIRE(std::get<int>(number.value) == 1);
        REQUIRE(std::get<char>(special.value) == '+');

        TEST_LOG << "Testing token equality";
        // Equality test
        Token name2 = { TokenType::name, "hello" };
        Token number2 = { TokenType::number, 1 };
        Token special2 = { TokenType::special, '+' };

        REQUIRE(name == name2);
        REQUIRE(number == number2);
        REQUIRE(special == special2);

        Token diffName = { TokenType::name, "world" };
        Token diffNumber = { TokenType::number, 2 };
        Token diffSpecial = { TokenType::special, '-' };

        REQUIRE_FALSE(name == diffName);
        REQUIRE_FALSE(number == diffNumber);
        REQUIRE_FALSE(special == diffSpecial);

        TEST_LOG << "Testing token assignment";
        // Assign test
        name2 = diffName;
        REQUIRE_FALSE(name == name2);
        REQUIRE(name2 == diffName);

        number2 = diffNumber;
        REQUIRE_FALSE(number == number2);
        REQUIRE(number2 == diffNumber);

        special2 = diffSpecial;
        REQUIRE_FALSE(special == special2);
        REQUIRE(special2 == diffSpecial);
    }

    TEST_CASE("Lexer test") {
        SECTION("Positive test") {
            std::string source = R"(procedure name{
                read y;
                x = y     + 1234;})";


            std::queue<Token> tokens;
            tokens.push(Token{ TokenType::name, "procedure" });
            tokens.push(Token{ TokenType::name, "name" });
            tokens.push(Token{ TokenType::special, '{' });

            tokens.push(Token{ TokenType::name, "read" });
            tokens.push(Token{ TokenType::name, "y" });
            tokens.push(Token{ TokenType::special, ';' });

            tokens.push(Token{ TokenType::name, "x" });
            tokens.push(Token{ TokenType::special, '=' });
            tokens.push(Token{ TokenType::name, "y" });
            tokens.push(Token{ TokenType::special, '+' });
            tokens.push(Token{ TokenType::number, 1234 });
            tokens.push(Token{ TokenType::special, ';' });
            tokens.push(Token{ TokenType::special, '}' });
            tokens.push(Token{ TokenType::eof, EOF });

            std::deque<Token> lexedTokens = Lexer(source).getTokens();


            REQUIRE(tokens.size() == lexedTokens.size());
            while (!tokens.empty()) {
                REQUIRE(lexedTokens.front() == tokens.front());
                lexedTokens.pop_front();
                tokens.pop();
            }

            REQUIRE_NOTHROW(Lexer("4+3*2/(1-5)-6%8").getTokens());
        }
    }

    TEST_CASE("Lexer rejects numbers starting with 0") {
        REQUIRE_THROWS_AS(Lexer("monke = 0420;").getTokens(), std::invalid_argument);
        REQUIRE_THROWS_AS(Lexer("if (monke == 069) { read monke; }").getTokens(), std::invalid_argument);
        REQUIRE_NOTHROW(Lexer("monke = 0;").getTokens());
    }

}  // namespace parser
}  // namespace sp
