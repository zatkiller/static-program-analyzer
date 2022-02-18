
#include <iostream>

#include "catch.hpp"
#include "Parser/AST.h"
#include "Parser/Lexer.h"
#include "Parser/Parser.h"

namespace sp {
namespace parser {

TEST_CASE("Testing Parser") {
    SECTION("Unit testing") {
        std::deque<Token> tokens;
        using ast::make;
        std::unique_ptr<ast::ASTNode> ast, expected;
        lineCount = 1;  // resetting line count

        SECTION("ExprParser::parse") {
            // var only basic expression
            tokens = Lexer("ooga / booga").getTokens();
            ast = ExprParser::parse(tokens);
            expected = make<ast::BinExpr>(
                ast::BinOp::DIVIDE,
                make<ast::Var>("ooga"),
                make<ast::Var>("booga")
            );
            REQUIRE(*ast == *expected);

            // const only basic expression
            tokens = Lexer("17 * 42 / 5").getTokens();
            ast = ExprParser::parse(tokens);
            expected = make<ast::BinExpr>(
                ast::BinOp::DIVIDE,
                make<ast::BinExpr>(
                    ast::BinOp::MULT,
                    make<ast::Const>(17),
                    make<ast::Const>(42)
                ),
                make<ast::Const>(5)
            );
            REQUIRE(*ast == *expected);
            
            // basic expression with var and const
            tokens = Lexer("ooga + 1 * booga").getTokens();
            ast = ExprParser::parse(tokens);
            expected = make<ast::BinExpr>(
                ast::BinOp::PLUS,
                make<ast::Var>("ooga"),
                make<ast::BinExpr>(
                    ast::BinOp::MULT,
                    make<ast::Const>(1),
                    make<ast::Var>("booga")
                )
            );
            REQUIRE(*ast == *expected);

            // basic expression nested in parenthesis
            tokens = Lexer("(((((ooga + 1)))))").getTokens();
            ast = ExprParser::parse(tokens);
            expected = make<ast::BinExpr>(
                ast::BinOp::PLUS,
                make<ast::Var>("ooga"),
                make<ast::Const>(1)
            );
            REQUIRE(*ast == *expected);

            // Should be able to parse subexpressions without brackets
            tokens = Lexer("x + y * z - 12 % 7 - x + z * ooga / booga").getTokens();
            expected = make<ast::BinExpr>(
                ast::BinOp::PLUS,
                make<ast::BinExpr>(
                    ast::BinOp::MINUS,
                    make<ast::BinExpr>(
                        ast::BinOp::MINUS,
                        make<ast::BinExpr>(
                            ast::BinOp::PLUS,
                            make<ast::Var>("x"),
                            make<ast::BinExpr>(
                                ast::BinOp::MULT,
                                make<ast::Var>("y"),
                                make<ast::Var>("z")
                            )
                        ),
                        make<ast::BinExpr>(
                            ast::BinOp::MOD,
                            make<ast::Const>(12),
                            make<ast::Const>(7)
                        )
                    ),
                    make<ast::Var>("x")
                ),
                make<ast::BinExpr>(
                    ast::BinOp::DIVIDE,
                    make<ast::BinExpr>(
                        ast::BinOp::MULT,
                        make<ast::Var>("z"),
                        make<ast::Var>("ooga")
                    ),
                    make<ast::Var>("booga")
                )
            );
            // Should be able to parse many subexpressions with brackets
            tokens = Lexer("(x + y * z) - (12 % 7 - x) + (z * ooga / booga)").getTokens();
            ast = ExprParser::parse(tokens);
            expected = make<ast::BinExpr>(
                ast::BinOp::PLUS,
                make<ast::BinExpr>(
                    ast::BinOp::MINUS,
                    make<ast::BinExpr>(
                        ast::BinOp::PLUS,
                        make<ast::Var>("x"),
                        make<ast::BinExpr>(
                            ast::BinOp::MULT,
                            make<ast::Var>("y"),
                            make<ast::Var>("z")
                        )
                    ),
                    make<ast::BinExpr>(
                        ast::BinOp::MINUS,
                        make<ast::BinExpr>(
                            ast::BinOp::MOD,
                            make<ast::Const>(12),
                            make<ast::Const>(7)
                        ),
                        make<ast::Var>("x")
                    )
                ),
                make<ast::BinExpr>(
                    ast::BinOp::DIVIDE,
                    make<ast::BinExpr>(
                        ast::BinOp::MULT,
                        make<ast::Var>("z"),
                        make<ast::Var>("ooga")
                    ),
                    make<ast::Var>("booga")
                )
            );

            // Should be able to parse many subexpressions with and without brackets
            REQUIRE(*ast == *expected);
            tokens = Lexer("4+x*2/(1-y)-6%8").getTokens();
            ast = ExprParser::parse(tokens);
            expected = make<ast::BinExpr>(
                ast::BinOp::MINUS,
                make<ast::BinExpr>(
                    ast::BinOp::PLUS,
                    make<ast::Const>(4),
                    make<ast::BinExpr>(
                        ast::BinOp::DIVIDE,
                        make<ast::BinExpr>(
                            ast::BinOp::MULT,
                            make<ast::Var>("x"),
                            make<ast::Const>(2)
                        ),
                        make<ast::BinExpr>(
                            ast::BinOp::MINUS,
                            make<ast::Const>(1),
                            make<ast::Var>("y")
                        )
                    )
                ),
                make<ast::BinExpr>(
                    ast::BinOp::MOD,
                    make<ast::Const>(6),
                    make<ast::Const>(8)
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
                return make<ast::RelExpr>(
                    ast::RelOp::GT,
                    make<ast::BinExpr>(ast::BinOp::PLUS, make<ast::Var>("x"), make<ast::Const>(1)),
                    make<ast::BinExpr>(ast::BinOp::PLUS, make<ast::Var>("y"), make<ast::Const>(2))
                );
            };
            auto relExpr2 = []() {
                return make<ast::RelExpr>(
                    ast::RelOp::GT,
                    make<ast::BinExpr>(ast::BinOp::PLUS, make<ast::Var>("z"), make<ast::Const>(1)),
                    make<ast::BinExpr>(ast::BinOp::PLUS, make<ast::Var>("t"), make<ast::Const>(2))
                );
            };
            // parse basic rel expression
            tokens = Lexer("x > y").getTokens();
            ast = CondExprParser::parse(tokens);
            expected = make<ast::RelExpr>(
                ast::RelOp::GT,
                make<ast::Var>("x"),
                make<ast::Var>("y")
            );
            REQUIRE(*ast == *expected);


            // nested cond expression with &&
            tokens = Lexer("((x+1)>(y+2))&&((z+1)>(t+2))").getTokens();
            std::unique_ptr<ast::CondExpr> ast = CondExprParser::parse(tokens);
            std::unique_ptr<ast::CondExpr> expected = make<ast::CondBinExpr>(
                ast::CondOp::AND,
                relExpr1(),
                relExpr2()
            );
            REQUIRE(*ast == *expected);

            // nested cond expression with ||
            tokens = Lexer("((x+1)>(y+2))||((z+1)>(t+2))").getTokens();
            ast = CondExprParser::parse(tokens);
            expected = make<ast::CondBinExpr>(
                ast::CondOp::OR,
                relExpr1(),
                relExpr2()
            );
            REQUIRE(*ast == *expected);

            // nested cond expression with !
            tokens = Lexer("!((x+1)>(y+2))").getTokens();
            ast = CondExprParser::parse(tokens);
            expected = std::make_unique<ast::NotCondExpr>(
                relExpr1()
            );
            REQUIRE(*ast == *expected);

            // 
        }

        SECTION("StmtLstParser::parse") {
            tokens = Lexer(R"(
                read v1;
                print v1;
                v2 = v1;
            )").getTokens();

            auto stmtlst = StmtLstParser::parse(tokens);
            auto expectedStmtlst = ast::makeStmts(
                make<ast::Read>(1, make<ast::Var>("v1")),
                make<ast::Print>(2, make<ast::Var>("v1")),
                make<ast::Assign>(3, make<ast::Var>("v2"), make<ast::Var>("v1"))
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
                return ast::makeStmts(
                    make<ast::Read>(1, make<ast::Var>("v1")),
                    make<ast::Print>(2, make<ast::Var>("v1")),
                    make<ast::Assign>(3, make<ast::Var>("v2"), make<ast::Var>("v1"))
                );
            };
            expected = make<ast::Procedure>(
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
                return ast::makeStmts(
                    make<ast::Read>(1, make<ast::Var>("v1")),
                    make<ast::Print>(2, make<ast::Var>("v1")),
                    make<ast::Assign>(3, make<ast::Var>("v2"), make<ast::Var>("v1"))
                );
            };
            expected = std::make_unique<ast::Program>(make<ast::Procedure>(
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

}  // namespace parser
}  // namespace sp
