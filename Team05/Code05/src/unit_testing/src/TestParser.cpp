
#include <iostream>

#include "catch.hpp"
#include "Parser/AST.h"
#include "Parser/Lexer.h"
#include "Parser/Parser.h"

namespace SimpleParser {

TEST_CASE("Testing Parser") {
    SECTION("Unit testing") {
        std::deque<Token> tokens;
        using AST::make;
        std::unique_ptr<AST::ASTNode> ast, expected;
        lineCount = 1;

        SECTION("ExprParser::parse") {
            tokens = Lexer("4+x*2/(1-y)-6%8").getTokens();
            ast = ExprParser::parse(tokens);
            expected = make<AST::BinExpr>(
                AST::BinOp::MINUS,
                make<AST::BinExpr>(
                    AST::BinOp::PLUS,
                    make<AST::Const>(4),
                    make<AST::BinExpr>(
                        AST::BinOp::DIVIDE,
                        make<AST::BinExpr>(
                            AST::BinOp::MULT,
                            make<AST::Var>("x"),
                            make<AST::Const>(2)
                        ),
                        make<AST::BinExpr>(
                            AST::BinOp::MINUS,
                            make<AST::Const>(1),
                            make<AST::Var>("y")
                        )
                    )
                ),
                make<AST::BinExpr>(
                    AST::BinOp::MOD,
                    make<AST::Const>(6),
                    make<AST::Const>(8)
                )
            );
            REQUIRE(*ast == *expected);
            

            // Left associativity test
            tokens = Lexer("4+3+2").getTokens();
            auto ast1 = ExprParser::parse(tokens);
            tokens = Lexer("(4+3)+2").getTokens();
            auto ast2 = ExprParser::parse(tokens);
            tokens = Lexer("4+(3+2)").getTokens();
            auto ast3 = ExprParser::parse(tokens);
            REQUIRE(*ast1 == *ast2);
            REQUIRE(!(*ast1 == *ast3));
        }
        
        SECTION("CondExprParser::parse") {
            auto relExpr1 = []() {
                return make<AST::RelExpr>(
                    AST::RelOp::GT,
                    make<AST::BinExpr>(AST::BinOp::PLUS, make<AST::Var>("x"), make<AST::Const>(1)),
                    make<AST::BinExpr>(AST::BinOp::PLUS, make<AST::Var>("y"), make<AST::Const>(2))
                );
            };
            auto relExpr2 = []() {
                return make<AST::RelExpr>(
                    AST::RelOp::GT,
                    make<AST::BinExpr>(AST::BinOp::PLUS, make<AST::Var>("z"), make<AST::Const>(1)),
                    make<AST::BinExpr>(AST::BinOp::PLUS, make<AST::Var>("t"), make<AST::Const>(2))
                );
            };

            tokens = Lexer("((x+1)>(y+2))&&((z+1)>(t+2))").getTokens();
            std::unique_ptr<AST::CondExpr> ast = CondExprParser::parse(tokens);
            std::unique_ptr<AST::CondExpr> expected = make<AST::CondBinExpr>(
                AST::CondOp::AND,
                relExpr1(),
                relExpr2()
            );
            REQUIRE(*ast == *expected);

            tokens = Lexer("((x+1)>(y+2))||((z+1)>(t+2))").getTokens();
            ast = CondExprParser::parse(tokens);
            expected = make<AST::CondBinExpr>(
                AST::CondOp::OR,
                relExpr1(),
                relExpr2()
            );
            REQUIRE(*ast == *expected);

            tokens = Lexer("!((x+1)>(y+2))").getTokens();
            ast = CondExprParser::parse(tokens);
            expected = std::make_unique<AST::NotCondExpr>(
                relExpr1()
            );
            REQUIRE(*ast == *expected);
        }

        SECTION("StmtLstParser::parse") {
            tokens = Lexer(R"(
                read v1;
                print v1;
                v2 = v1;
            )").getTokens();

            auto stmtlst = StmtLstParser::parse(tokens);
            auto expectedStmtlst = AST::makeStmts(
                make<AST::Read>(1, make<AST::Var>("v1")),
                make<AST::Print>(2, make<AST::Var>("v1")),
                make<AST::Assign>(3, make<AST::Var>("v2"), make<AST::Var>("v1"))
            );
            REQUIRE(stmtlst == expectedStmtlst);
        }
        
        SECTION("parseProcedure") {
            tokens = Lexer(R"(
            procedure main {
                read v1;
                print v1;
                v2 = v1;
            }
            )").getTokens();

            ast = parseProcedure(tokens);
            auto genStmtlst = []() {
                return AST::makeStmts(
                    make<AST::Read>(1, make<AST::Var>("v1")),
                    make<AST::Print>(2, make<AST::Var>("v1")),
                    make<AST::Assign>(3, make<AST::Var>("v2"), make<AST::Var>("v1"))
                );
            };
            expected = make<AST::Procedure>(
                "main",
                genStmtlst()
            );

            REQUIRE(*ast == *expected);
        }
        
        SECTION("parseProgram") {
            tokens = Lexer(R"(
            procedure main {
                read v1;
                print v1;
                v2 = v1;
            }
            )").getTokens();

            ast = parseProgram(tokens);

            auto genStmtlst = []() {
                return AST::makeStmts(
                    make<AST::Read>(1, make<AST::Var>("v1")),
                    make<AST::Print>(2, make<AST::Var>("v1")),
                    make<AST::Assign>(3, make<AST::Var>("v2"), make<AST::Var>("v1"))
                );
            };
            expected = std::make_unique<AST::Program>(make<AST::Procedure>(
                "main",
                genStmtlst()
            ));

            REQUIRE(*ast == *expected);
        }

    }

    SECTION("Complete procedure test") {
        std::string fail = R"(procedure a {
            while (c !! b) {
                read c; 
            }
        })";
        REQUIRE(parse(fail) == nullptr);

        std::string fail2 = R"(procedure a {
            while (!c)) {
                read c;
            }
        })";
        REQUIRE(parse(fail2) == nullptr);


        std::string fail3 = R"(procedure 1a {
            1x = 1y + 1z;
        })";
        REQUIRE(parse(fail3) == nullptr);

        std::string testCode = R"(procedure computeAverage {
            read num1;
            read num2;
            read num3;
            sum = num1 + num2 + num3;
            ave = sum / 3;
            print ave;
        })";
        REQUIRE(parse(testCode) != nullptr);


        std::string testCode2 = R"(procedure printAscending {
            read num1;
            read num2;
            noSwap = 0;
            if (num1 > num2) then{
                temp = num1;
                num1 = num2;
                num2 = temp;
            }
            else {
                noSwap = 1;
            }
            print num1;
            print num2;
            print noSwap;
        })";
        REQUIRE(parse(testCode2) != nullptr);

        std::string generatedCode1 = R"(procedure Y5Gw {
            if ((0 >= 758) && (!(34 >= 5))) then {
                z955ACp = (6018) * E1IOMsY;
                read y7633lWD;
                qR84sPE9 = 3518 + mZ8pf;
            } else {
                while (!(!(63 >= 0))) {
                    print pGcpOwD;
                    Hv37 = 290 * (553) + 38 * 3;
                }
            }
    
        })";
        REQUIRE(parse(generatedCode1) != nullptr);

    }    
}

}  // namespace SimpleParser

