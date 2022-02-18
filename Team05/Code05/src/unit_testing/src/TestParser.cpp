
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
            ast = expr_parser::parse(tokens);
            expected = make<ast::BinExpr>(
                ast::BinOp::DIVIDE,
                make<ast::Var>("ooga"),
                make<ast::Var>("booga")
            );
            REQUIRE(*ast == *expected);

            // const only basic expression
            tokens = Lexer("17 * 42 / 5").getTokens();
            ast = expr_parser::parse(tokens);
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
            ast = expr_parser::parse(tokens);
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
            ast = expr_parser::parse(tokens);
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
            ast = expr_parser::parse(tokens);
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
            ast = expr_parser::parse(tokens);
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
            auto ast1 = expr_parser::parse(tokens);
            tokens = Lexer("(4+3)+2").getTokens();
            auto ast2 = expr_parser::parse(tokens);
            tokens = Lexer("4+(3+2)").getTokens();
            auto ast3 = expr_parser::parse(tokens);
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
            // basic rel expressions for ANY rel of the rel symbols
            tokens = Lexer("x == y").getTokens();
            ast = cond_expr_parser::parse(tokens);
            expected = make<ast::RelExpr>(
                ast::RelOp::EQ,
                make<ast::Var>("x"),
                make<ast::Var>("y")
            );
            REQUIRE(*ast == *expected);

            tokens = Lexer("x > y").getTokens();
            ast = cond_expr_parser::parse(tokens);
            expected = make<ast::RelExpr>(
                ast::RelOp::GT,
                make<ast::Var>("x"),
                make<ast::Var>("y")
            );
            REQUIRE(*ast == *expected);

            tokens = Lexer("x >= y").getTokens();
            ast = cond_expr_parser::parse(tokens);
            expected = make<ast::RelExpr>(
                ast::RelOp::GTE,
                make<ast::Var>("x"),
                make<ast::Var>("y")
            );
            REQUIRE(*ast == *expected);

            tokens = Lexer("x < y").getTokens();
            ast = cond_expr_parser::parse(tokens);
            expected = make<ast::RelExpr>(
                ast::RelOp::LT,
                make<ast::Var>("x"),
                make<ast::Var>("y")
            );
            REQUIRE(*ast == *expected);

            tokens = Lexer("x <= y").getTokens();
            ast = cond_expr_parser::parse(tokens);
            expected = make<ast::RelExpr>(
                ast::RelOp::LTE,
                make<ast::Var>("x"),
                make<ast::Var>("y")
            );
            REQUIRE(*ast == *expected);

            tokens = Lexer("x != y").getTokens();
            ast = cond_expr_parser::parse(tokens);
            expected = make<ast::RelExpr>(
                ast::RelOp::NE,
                make<ast::Var>("x"),
                make<ast::Var>("y")
            );
            REQUIRE(*ast == *expected);

            // nested cond expression with &&
            tokens = Lexer("((x+1)>(y+2))&&((z+1)>(t+2))").getTokens();
            std::unique_ptr<ast::CondExpr> ast = cond_expr_parser::parse(tokens);
            std::unique_ptr<ast::CondExpr> expected = make<ast::CondBinExpr>(
                ast::CondOp::AND,
                relExpr1(),
                relExpr2()
            );
            REQUIRE(*ast == *expected);

            // nested cond expression with ||
            tokens = Lexer("((x+1)>(y+2))||((z+1)>(t+2))").getTokens();
            ast = cond_expr_parser::parse(tokens);
            expected = make<ast::CondBinExpr>(
                ast::CondOp::OR,
                relExpr1(),
                relExpr2()
            );
            REQUIRE(*ast == *expected);

            // nested cond expression with !
            tokens = Lexer("!((x+1)>(y+2))").getTokens();
            ast = cond_expr_parser::parse(tokens);
            expected = std::make_unique<ast::NotCondExpr>(
                relExpr1()
            );
            REQUIRE(*ast == *expected);

            // heavily nested cond expression
            tokens = Lexer("((!((x+1)>(y+2)))&&((z+1)>(t+2)))||((x+1)>(y+2))").getTokens();
            ast = cond_expr_parser::parse(tokens);
            expected = make<ast::CondBinExpr>(
                ast::CondOp::OR,
                make<ast::CondBinExpr>(
                    ast::CondOp::AND,
                    std::make_unique<ast::NotCondExpr>(relExpr1()),
                    relExpr2()
                ),
                relExpr1()
            );
            REQUIRE(*ast == *expected);
        }

        SECTION("StmtLstParser::parse") {
            auto basicStmtLst = [](int stmtNo1, int stmtNo2, int stmtNo3) {
                return ast::makeStmts(
                    make<ast::Read>(stmtNo1, make<ast::Var>("v1")),
                    make<ast::Print>(stmtNo2, make<ast::Var>("v1")),
                    make<ast::Assign>(stmtNo3, make<ast::Var>("v2"), make<ast::Var>("v1"))
                );
            };
            // Testing with all currently possible statements: read, print, assign, while, if
            // We test if the 5 statements work with multiple levels of nesting (0, 1 and 2)
            tokens = Lexer(R"(
                read v1;
                print v1;
                v2 = v1;
                if ((x * y == 6) && (x / y != 9)) then {
                    read v1;
                    print v1;
                    v2 = v1;
                    if ((x * y == 6) && (x / y != 9)) then {
                        read v1;
                        print v1;
                        v2 = v1;
                    } else {
                        read v1;
                        print v1;
                        v2 = v1;
                    }
                    while (x < 420) {
                        read v1;
                        print v1;
                        v2 = v1;
                    }
                } else {
                    read v1;
                    print v1;
                    v2 = v1;
                }
                while (x < 420) {
                    read v1;
                    print v1;
                    v2 = v1;
                    if ((x * y == 6) && (x / y != 9)) then {
                        read v1;
                        print v1;
                        v2 = v1;
                    } else {
                        read v1;
                        print v1;
                        v2 = v1;
                    }
                    while (x < 420) {
                        read v1;
                        print v1;
                        v2 = v1;
                    }
                }
            )").getTokens();
            auto stmtlst = statement_list_parser::parse(tokens);
            auto expectedStmtlst = ast::makeStmts(
                make<ast::Read>(1, make<ast::Var>("v1")),
                make<ast::Print>(2, make<ast::Var>("v1")),
                make<ast::Assign>(3, make<ast::Var>("v2"), make<ast::Var>("v1")),
                make<ast::If>(4, 
                    make<ast::CondBinExpr>(
                        ast::CondOp::AND,
                        make<ast::RelExpr>(
                            ast::RelOp::EQ,
                            make<ast::BinExpr>(ast::BinOp::MULT, make<ast::Var>("x"), make<ast::Var>("y")),
                            make<ast::Const>(6)
                        ),
                        make<ast::RelExpr>(
                            ast::RelOp::NE,
                            make<ast::BinExpr>(ast::BinOp::DIVIDE, make<ast::Var>("x"), make<ast::Var>("y")),
                            make<ast::Const>(9)
                        ) 
                    ),
                    ast::makeStmts(
                        make<ast::Read>(5, make<ast::Var>("v1")),
                        make<ast::Print>(6, make<ast::Var>("v1")),
                        make<ast::Assign>(7, make<ast::Var>("v2"), make<ast::Var>("v1")),
                        make<ast::If>(8,
                            make<ast::CondBinExpr>(
                                ast::CondOp::AND,
                                make<ast::RelExpr>(
                                    ast::RelOp::EQ,
                                    make<ast::BinExpr>(ast::BinOp::MULT, make<ast::Var>("x"), make<ast::Var>("y")),
                                    make<ast::Const>(6)
                                ),
                                make<ast::RelExpr>(
                                    ast::RelOp::NE,
                                    make<ast::BinExpr>(ast::BinOp::DIVIDE, make<ast::Var>("x"), make<ast::Var>("y")),
                                    make<ast::Const>(9)
                                ) 
                            ),
                            basicStmtLst(9, 10, 11), 
                            basicStmtLst(12, 13, 14)
                        ),
                        make<ast::While>(15,
                            make<ast::RelExpr>(
                                ast::RelOp::LT,
                                make<ast::Var>("x"),
                                make<ast::Const>(420)
                            ),
                            basicStmtLst(16, 17, 18)
                        )
                    ),
                    basicStmtLst(19, 20, 21)
                ),
                make<ast::While>(22, 
                    make<ast::RelExpr>(
                        ast::RelOp::LT,
                        make<ast::Var>("x"),
                        make<ast::Const>(420)
                    ),
                    ast::makeStmts(
                        make<ast::Read>(23, make<ast::Var>("v1")),
                        make<ast::Print>(24, make<ast::Var>("v1")),
                        make<ast::Assign>(25, make<ast::Var>("v2"), make<ast::Var>("v1")),
                        make<ast::If>(26,
                            make<ast::CondBinExpr>(
                                ast::CondOp::AND,
                                make<ast::RelExpr>(
                                    ast::RelOp::EQ,
                                    make<ast::BinExpr>(ast::BinOp::MULT, make<ast::Var>("x"), make<ast::Var>("y")),
                                    make<ast::Const>(6)
                                ),
                                make<ast::RelExpr>(
                                    ast::RelOp::NE,
                                    make<ast::BinExpr>(ast::BinOp::DIVIDE, make<ast::Var>("x"), make<ast::Var>("y")),
                                    make<ast::Const>(9)
                                )
                            ),
                            basicStmtLst(27, 28, 29),
                            basicStmtLst(30, 31, 32)
                        ),
                        make<ast::While>(33,
                            make<ast::RelExpr>(
                                ast::RelOp::LT,
                                make<ast::Var>("x"),
                                make<ast::Const>(420)
                            ),
                            basicStmtLst(34, 35, 36)
                        )
                    )
                )
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

    SECTION("Complete program test") {
        using ast::make;
        
        // Program with a bad operator
        std::string fail = R"(
            procedure a {
                while (c !! b) {
                    read c; 
                }
            }
        )";
        REQUIRE(parse(fail) == nullptr);

        // Corrected program
        lineCount = 1; // reset lineCount
        std::string correct = R"(
            procedure a {
                while (c != b) {
                    read c; 
                }
            }
        )";
        auto parsedCorrect = std::make_unique<ast::Program>(
            make<ast::Procedure>(
                "a",
                ast::makeStmts(
                    make<ast::While>(1,
                        make<ast::RelExpr>(
                            ast::RelOp::NE,
                            make<ast::Var>("c"),
                            make<ast::Var>("b")
                        ),
                        ast::makeStmts(
                            make<ast::Read>(2, make<ast::Var>("c"))
                        )
                    )
                )
            )
        );
        REQUIRE(*parse(correct) == *parsedCorrect);

        // Program with incorrect parenthesis
        lineCount = 1; // reset lineCount
        std::string fail2 = R"(procedure a {
            while (!(c == 1))) {
                read c;
            }
        })";
        REQUIRE(parse(fail2) == nullptr);
        
        // corrected program
        lineCount = 1; // reset lineCount
        std::string correct2 = R"(procedure a {
            while (!(c == 1)) {
                read c;
            }
        })";
        auto parsedCorrect2 = std::make_unique<ast::Program>(
            make<ast::Procedure>(
                "a",
                ast::makeStmts(
                    make<ast::While>(1,
                        std::make_unique<ast::NotCondExpr>(
                            make<ast::RelExpr>(
                                ast::RelOp::EQ,
                                make<ast::Var>("c"),
                                make<ast::Const>(1)
                            )
                        ),
                        ast::makeStmts(make<ast::Read>(2, make<ast::Var>("c")))
                    )
                )
            )
        );
        REQUIRE(*parse(correct2) == *parsedCorrect2);

        // program with invalid variable names
        lineCount = 1; // reset lineCount
        std::string fail3 = R"(procedure 1a {
            1x = 1y + 1z;
        })";
        REQUIRE(parse(fail3) == nullptr);

        lineCount = 1; // reset lineCount
        std::string testCode = R"(procedure computeAverage {
            read num1;
            read num2;
            read num3;
            sum = num1 + num2 + num3;
            ave = sum / 3;
            print ave;
        })";
        auto parsedTestCode = std::make_unique<ast::Program>(
            make<ast::Procedure>(
                "computeAverage",
                ast::makeStmts(
                    make<ast::Read>(1, make<ast::Var>("num1")),
                    make<ast::Read>(2, make<ast::Var>("num2")),
                    make<ast::Read>(3, make<ast::Var>("num3")),
                    make<ast::Assign>(4, 
                        make<ast::Var>("sum"), 
                        make<ast::BinExpr>(
                            ast::BinOp::PLUS,
                            make<ast::BinExpr>(
                                ast::BinOp::PLUS,
                                make<ast::Var>("num1"),
                                make<ast::Var>("num2")
                            ), 
                            make<ast::Var>("num3")
                        )
                    ),
                    make<ast::Assign>(5,
                        make<ast::Var>("ave"),
                        make<ast::BinExpr>(
                            ast::BinOp::DIVIDE,
                            make<ast::Var>("sum"),
                            make<ast::Const>(3)
                        )
                    ),
                    make<ast::Print>(6, make<ast::Var>("ave"))
                )
            )
        );
        REQUIRE(*parse(testCode) == *parsedTestCode);

        lineCount = 1; // reset lineCount
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
        auto parsedTestCode2 = std::make_unique<ast::Program>(
            make<ast::Procedure>(
                "printAscending",
                ast::makeStmts(
                    make<ast::Read>(1, make<ast::Var>("num1")),
                    make<ast::Read>(2, make<ast::Var>("num2")),
                    make<ast::Assign>(3, 
                        make<ast::Var>("noSwap"), 
                        make<ast::Const>(0)
                    ),
                    make<ast::If>(4,
                        make<ast::RelExpr>(
                            ast::RelOp::GT,
                            make<ast::Var>("num1"),
                            make<ast::Var>("num2")
                        ),
                        ast::makeStmts(
                            make<ast::Assign>(5, 
                                make<ast::Var>("temp"), 
                                make<ast::Var>("num1")
                            ),
                            make<ast::Assign>(6, 
                                make<ast::Var>("num1"), 
                                make<ast::Var>("num2")
                            ),
                            make<ast::Assign>(7, 
                                make<ast::Var>("num2"), 
                                make<ast::Var>("temp")
                            )
                        ),
                        ast::makeStmts(
                            make<ast::Assign>(8, 
                                make<ast::Var>("noSwap"), 
                                make<ast::Const>(1)
                            )
                        )
                    ),
                    make<ast::Print>(9, make<ast::Var>("num1")),
                    make<ast::Print>(10, make<ast::Var>("num2")),
                    make<ast::Print>(11, make<ast::Var>("noSwap"))
                )
            )
        );
        REQUIRE(*parse(testCode2) == *parsedTestCode2);
    }    
}

}  // namespace parser
}  // namespace sp
