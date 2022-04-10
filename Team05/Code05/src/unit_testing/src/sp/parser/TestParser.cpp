
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
        // reset state
        lineCount = 1;
        callStmts.clear();
        procedures.clear();
        

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
            expected = makeProgram(make<ast::Procedure>("main", std::move(genStmtlst())));

            REQUIRE(*ast == *expected);

            // reset state.
            lineCount = 1;
            callStmts.clear();
            procedures.clear();
            // testing a program with 2 procedures
            tokens = Lexer(R"(
            procedure main {
                read v1;
                print v1;
                v2 = v1;
                call monke;
            }
            procedure monke {
                read v3;
                v2 = v3;
            }
            )").getTokens();

            ast = parseProgram(tokens);

            expected = makeProgram(
                make<ast::Procedure>(
                    "main", 
                    ast::makeStmts(
                        make<ast::Read>(1, make<ast::Var>("v1")),
                        make<ast::Print>(2, make<ast::Var>("v1")),
                        make<ast::Assign>(3, make<ast::Var>("v2"), make<ast::Var>("v1")),
                        make<ast::Call>(4, "monke")
                    )
                ),
                make<ast::Procedure>(
                    "monke", 
                    ast::makeStmts(
                        make<ast::Read>(5, make<ast::Var>("v3")),
                        make<ast::Assign>(6, make<ast::Var>("v2"), make<ast::Var>("v3"))
                    )
                )
            );

            REQUIRE(*ast == *expected);

            // reset state.
            lineCount = 1;
            callStmts.clear();
            procedures.clear();
            // testing a simple program with >2 procedures
            tokens = Lexer(R"(
            procedure main {
                read v1;
                print v1;
                call banana;
                v2 = v1;
                call monke;
            }
            procedure monke {
                read v3;
                v2 = v3;
            }
            procedure banana {
                read v2;
                print v2;
            }
            )").getTokens();

            ast = parseProgram(tokens);

            expected = makeProgram(
                make<ast::Procedure>(
                    "main", 
                    ast::makeStmts(
                        make<ast::Read>(1, make<ast::Var>("v1")),
                        make<ast::Print>(2, make<ast::Var>("v1")),
                        make<ast::Call>(3, "banana"),
                        make<ast::Assign>(4, make<ast::Var>("v2"), make<ast::Var>("v1")),
                        make<ast::Call>(5, "monke")
                    )
                ),
                make<ast::Procedure>(
                    "monke", 
                    ast::makeStmts(
                        make<ast::Read>(6, make<ast::Var>("v3")),
                        make<ast::Assign>(7, make<ast::Var>("v2"), make<ast::Var>("v3"))
                    )
                ),
                make<ast::Procedure>(
                    "banana", 
                    ast::makeStmts(
                        make<ast::Read>(8, make<ast::Var>("v2")),
                        make<ast::Print>(9, make<ast::Var>("v2"))
                    )
                )
            );

            REQUIRE(*ast == *expected);

            // reset state.
            lineCount = 1;
            callStmts.clear();
            procedures.clear();
            // testing a heavily-nested program with >2 procedures
            tokens = Lexer(R"(
            procedure monke {
                read v1;
                print v1;
                call banana;
                while (v1 != v2 * v1) {
                    v2 = v1;
                    if (v2 != v3) then {
                        while (v3 != v1) {
                            read v3;
                            read v2;
                            read v1;
                            while (v2 != v1) {
                                call eat;
                            }
                        }
                        call banana;
                    } else {
                        print v3;
                        print v2;
                        print v1;
                    }
                    call eat;
                }
            }
            procedure eat {
                v4 = 69;
                while (v4 < 420) {
                    v4 = v4 * 2;
                }
                read v3;
                v2 = v3;
            }
            procedure banana {
                while (v5 < 420) {
                    v5 = v5 * 2;
                }
                read v5;
                print v5;
            }
            )").getTokens();

            ast = parseProgram(tokens);

            expected = makeProgram(
                make<ast::Procedure>(
                    "monke", 
                    ast::makeStmts(
                        make<ast::Read>(1, make<ast::Var>("v1")),
                        make<ast::Print>(2, make<ast::Var>("v1")),
                        make<ast::Call>(3, "banana"),
                        make<ast::While>(4, 
                            make<ast::RelExpr>(
                                ast::RelOp::NE,
                                make<ast::Var>("v1"),
                                make<ast::BinExpr>(
                                    ast::BinOp::MULT,
                                    make<ast::Var>("v2"),
                                    make<ast::Var>("v1")
                                )
                            ),
                            ast::makeStmts(
                                make<ast::Assign>(
                                    5, 
                                    make<ast::Var>("v2"),
                                    make<ast::Var>("v1")
                                ),
                                make<ast::If>(
                                    6,
                                    make<ast::RelExpr>(
                                        ast::RelOp::NE,
                                        make<ast::Var>("v2"),
                                        make<ast::Var>("v3")
                                    ),
                                    ast::makeStmts(
                                        make<ast::While>(
                                            7,
                                            make<ast::RelExpr>(
                                                ast::RelOp::NE,
                                                make<ast::Var>("v3"),
                                                make<ast::Var>("v1")
                                            ),
                                            ast::makeStmts(
                                                make<ast::Read>(8, make<ast::Var>("v3")),
                                                make<ast::Read>(9, make<ast::Var>("v2")),
                                                make<ast::Read>(10, make<ast::Var>("v1")),
                                                make<ast::While>(
                                                    11, 
                                                    make<ast::RelExpr>(
                                                        ast::RelOp::NE,
                                                        make<ast::Var>("v2"),
                                                        make<ast::Var>("v1")
                                                    ),
                                                    ast::makeStmts(
                                                        make<ast::Call>(12, "eat")
                                                    )
                                                )
                                            )
                                        ),
                                        make<ast::Call>(13, "banana")
                                    ),
                                    ast::makeStmts(
                                        make<ast::Print>(14, make<ast::Var>("v3")),
                                        make<ast::Print>(15, make<ast::Var>("v2")),
                                        make<ast::Print>(16, make<ast::Var>("v1"))
                                    )
                                ),
                                make<ast::Call>(17, "eat")
                            )
                        )
                    )
                ),
                make<ast::Procedure>(
                    "eat", 
                    ast::makeStmts(
                        make<ast::Assign>(18, make<ast::Var>("v4"), make<ast::Const>(69)),
                        make<ast::While>(
                            19, 
                            make<ast::RelExpr>(
                                ast::RelOp::LT, 
                                make<ast::Var>("v4"), 
                                make<ast::Const>(420)
                            ),
                            ast::makeStmts(
                                make<ast::Assign>(
                                    20,
                                    make<ast::Var>("v4"),
                                    make<ast::BinExpr>(
                                        ast::BinOp::MULT,
                                        make<ast::Var>("v4"),
                                        make<ast::Const>(2)
                                    )
                                )
                            )
                        ),
                        make<ast::Read>(
                            21, 
                            make<ast::Var>("v3")
                        ),
                        make<ast::Assign>(
                            22, 
                            make<ast::Var>("v2"), 
                            make<ast::Var>("v3")
                        )
                    )
                ),
                make<ast::Procedure>(
                    "banana", 
                    ast::makeStmts(
                        make<ast::While>(
                            23, 
                            make<ast::RelExpr>(
                                ast::RelOp::LT, 
                                make<ast::Var>("v5"), 
                                make<ast::Const>(420)
                            ),
                            ast::makeStmts(
                                make<ast::Assign>(
                                    24,
                                    make<ast::Var>("v5"),
                                    make<ast::BinExpr>(
                                        ast::BinOp::MULT,
                                        make<ast::Var>("v5"),
                                        make<ast::Const>(2)
                                    )
                                )
                            )
                        ),
                        make<ast::Read>(25, make<ast::Var>("v5")),
                        make<ast::Print>(26, make<ast::Var>("v5"))
                    )
                )
            );

            REQUIRE(*ast == *expected);
        }
    }

    SECTION("Complete program test") {
        using ast::make;
        
        std::string emptyProg = "";
        REQUIRE(parse(emptyProg) == nullptr);

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
        // reset state.
        lineCount = 1;
        callStmts.clear();
        procedures.clear();
        std::string correct = R"(
            procedure a {
                while (c != b) {
                    read c; 
                }
            }
        )";
        auto parsedCorrect = makeProgram(
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
        // reset state.
        lineCount = 1;
        callStmts.clear();
        procedures.clear();
        std::string fail2 = R"(procedure a {
            while (!(c == 1))) {
                read c;
            }
        })";
        REQUIRE(parse(fail2) == nullptr);
        
        // corrected program
        // reset state.
        lineCount = 1;
        callStmts.clear();
        procedures.clear();
        std::string correct2 = R"(procedure a {
            while (!(c == 1)) {
                read c;
            }
        })";
        auto parsedCorrect2 = makeProgram(
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
        // reset state.
        lineCount = 1;
        callStmts.clear();
        procedures.clear();
        std::string fail3 = R"(procedure 1a {
            1x = 1y + 1z;
        })";
        REQUIRE(parse(fail3) == nullptr);

        // reset state.
        lineCount = 1;
        callStmts.clear();
        procedures.clear();
        std::string testCode = R"(procedure computeAverage {
            read num1;
            read num2;
            read num3;
            sum = num1 + num2 + num3;
            ave = sum / 3;
            print ave;
        })";
        auto parsedTestCode = makeProgram(
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

        // reset state.
        lineCount = 1;
        callStmts.clear();
        procedures.clear();
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
        auto parsedTestCode2 = makeProgram(
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

TEST_CASE("Testing Exceptions Thrown and Source Line") {
    std::deque<Token> tokens;
    using ast::make;
    std::unique_ptr<ast::ASTNode> ast, expected;

    SECTION("Testing Exceptions") {
        // reset state
        lineCount = 1;
        callStmts.clear();
        procedures.clear();

        SECTION("Bad RelOp") {
            std::string badRelOpCode = R"(procedure a {
                
                
                
                while (c !! b) {
                    read c; 
                }
            })";
            auto badRelOpTokens = Lexer(badRelOpCode).getTokens();
            // Parser will expect the RelOp to be "!=" and the while stmt is on line 5.
            REQUIRE_THROWS_MATCHES(
                parseProgram(badRelOpTokens),
                std::invalid_argument,
                Catch::Message("CondExpr fails to parse at line: 5")
            );
        }

        SECTION("Bad call stmt") {
            // Testing bad call stmt with inappropriate procedure name
            std::string badCallCode = R"(procedure badCall {
                call 12345;
            })";
            auto badCallTokens = Lexer(badCallCode).getTokens();
            REQUIRE_THROWS_MATCHES(
                parseProgram(badCallTokens),
                std::invalid_argument,
                Catch::Message("Procedure name expected at line: 2")
            );
        }

        SECTION("Empty stmtlst") {
            // Testing bad stmtlst without statements
            std::string badStmtCode = R"(procedure bad {
                12345;
            })";
            auto badStmtTokens = Lexer(badStmtCode).getTokens();
            REQUIRE_THROWS_MATCHES(
                parseProgram(badStmtTokens),
                std::invalid_argument,
                Catch::Message("String expected at line: 2")
            );
        }

        SECTION("Empty program") {
            // Testing empty token list parsing 
            std::string emptyCode = "";
            auto emptyTokens = Lexer(emptyCode).getTokens();
            REQUIRE_THROWS_MATCHES(
                parseProgram(emptyTokens),
                std::invalid_argument,
                Catch::Message("Empty program received")
            );
        }

        SECTION("Bad keywords stmt") {
            // Testing possible bad keywords such as then and else
            std::string badKeywordCode = R"(procedure bad {
                if (x == y) notthenlol {
                    print ooga;
                } else {
                    print booga;
                }
            })";
            auto badKeywordTokens = Lexer(badKeywordCode).getTokens();
            REQUIRE_THROWS_MATCHES(
                parseProgram(badKeywordTokens),
                std::invalid_argument,
                Catch::Message("then expected at line: 2")
            );
        }
    
        SECTION("Missing variable stmt") {
            // Testing possible missing variables
            std::string noVarCode = R"(procedure bad {
                read;
            })";
            auto noVarTokens = Lexer(noVarCode).getTokens();
            REQUIRE_THROWS_MATCHES(
                parseProgram(noVarTokens),
                std::invalid_argument,
                Catch::Message("Name expected at line: 2")
            );
        }

        SECTION("parseRelOp exceptions") {
            // Testing case where the second '=' is missing
            std::string badRelOpCode = R"(procedure bad {
                while (1234 =monke= 1234) {
                    read monke;
                }
            })";
            auto badRelOpTokens = Lexer(badRelOpCode).getTokens();
            REQUIRE_THROWS_MATCHES(
                parseProgram(badRelOpTokens),
                std::invalid_argument,
                Catch::Message("CondExpr fails to parse at line: 2")
            );

            // reset state
            lineCount = 1;
            callStmts.clear();
            procedures.clear();
            // case where relOp is missing
            std::string badRelOpCode2 = R"(procedure bad {
                while (1234 )";
            auto badRelOpTokens2 = Lexer(badRelOpCode2).getTokens();
            REQUIRE_THROWS_MATCHES(
                parseProgram(badRelOpTokens2),
                std::invalid_argument,
                Catch::Message("CondExpr fails to parse at line: 2")
            );

            // reset state
            lineCount = 1;
            callStmts.clear();
            procedures.clear();
            // case where relOp is not recognised
            std::string badRelOpCode3 = R"(procedure bad {
                while (1234 ?? lol) {
                    read monke;
            })";
            auto badRelOpTokens3 = Lexer(badRelOpCode3).getTokens();
            REQUIRE_THROWS_MATCHES(
                parseProgram(badRelOpTokens3),
                std::invalid_argument,
                Catch::Message("CondExpr fails to parse at line: 2")
            );
        }

        SECTION("parseCondOp exceptions") {
            // Testing case where the second '&' is missing
            std::string badCondOpCode = R"(procedure bad {
                while ((1234 == 1234) &)";
            auto badCondOpTokens = Lexer(badCondOpCode).getTokens();
            REQUIRE_THROWS_MATCHES(
                parseProgram(badCondOpTokens),
                std::invalid_argument,
                Catch::Message("CondExpr fails to parse at line: 2")
            );

            // reset state
            lineCount = 1;
            callStmts.clear();
            procedures.clear();
            // Testing case where the second '|' is missing
            std::string badCondOpCode2 = R"(procedure bad {
                while ((1234 == 1234) |)";
            auto badCondOpTokens2 = Lexer(badCondOpCode2).getTokens();
            REQUIRE_THROWS_MATCHES(
                parseProgram(badCondOpTokens2),
                std::invalid_argument,
                Catch::Message("CondExpr fails to parse at line: 2")
            );

            // reset state
            lineCount = 1;
            callStmts.clear();
            procedures.clear();
            // Testing case where the condOp is missing
            std::string badCondOpCode3 = R"(procedure bad {
                while ((1234 == 1234) ?? (monke == monke)) {
                    read ooga;
            })";
            auto badCondOpTokens3 = Lexer(badCondOpCode3).getTokens();
            REQUIRE_THROWS_MATCHES(
                parseProgram(badCondOpTokens3),
                std::invalid_argument,
                Catch::Message("CondExpr fails to parse at line: 2")
            );

            // 2nd test case where the condOp is missing
            std::string badCondOpCode4 = R"(procedure bad {
                while ((1 + 2 + 3) && (monke == monke)) {
                    read ooga;
            })";
            auto badCondOpTokens4 = Lexer(badCondOpCode4).getTokens();
            REQUIRE_THROWS_MATCHES(
                parseProgram(badCondOpTokens4),
                std::invalid_argument,
                Catch::Message("CondExpr fails to parse at line: 2")
            );
        }

        SECTION("expr parsing exceptions") {
            std::string emptyCode = "";
            auto emptyTokens = Lexer(emptyCode).getTokens();
            REQUIRE_THROWS_MATCHES(
                expr_parser::parse(emptyTokens),
                std::invalid_argument,
                Catch::Message("More tokens (for Expr Parsing) expected at line: 1")
            );

            std::string missingOperandCode = "123 +";
            auto missingOperandTokens = Lexer(missingOperandCode).getTokens();
            REQUIRE_THROWS_MATCHES(
                expr_parser::parse(missingOperandTokens),
                std::invalid_argument,
                Catch::Message("More tokens (for Expr Parsing) expected")
            );
        }
    }

    SECTION("Additional Constraints") {
        lineCount = 1;
        callStmts.clear();
        procedures.clear();

        SECTION("Same name procedures should not be allowed") {
            // Same name procedures should not be allowed
            std::string problemCode1 = R"(procedure monke {
                    read v1;
                    print v1;
                    v2 = v1;
                    call monke;
                }
                procedure monke {
                    read v3;
                    v2 = v3;
            })";
            // The InvalidArgumentException will be thrown by parseProgram().
            auto tokens1 = Lexer(problemCode1).getTokens();
            // It will catch the repeated procedure "monke" at the 7th line of the source code
            REQUIRE_THROWS_MATCHES(
                parseProgram(tokens1),
                std::invalid_argument,
                Catch::Message("Repeated procedure name: \"monke\" at line: 7")
            );
            // It will be caught by the parse() method and return an empty Program.
            // Note: Need to lex again since tokens are consumed when parsed.
            tokens1 = Lexer(problemCode1).getTokens();
            REQUIRE(parse(problemCode1) == nullptr);
        }
        
        SECTION("Calling non-existent procedures should not be allowed") {
            // Calling non-existent procedures should not be allowed
            std::string problemCode2 = R"(procedure main {
                    read v1;
                    print v1;
                    v2 = v1;
                    call noExist;
                }
                procedure monke {
                    read v3;
                    v2 = v3;
            })";
            auto tokens2 = Lexer(problemCode2).getTokens();
            // It will catch the call for non-existent procedure "noExist" at the 5th line of the source code
            REQUIRE_THROWS_MATCHES(
                parseProgram(tokens2),
                std::invalid_argument,
                Catch::Message("Calling a non-existent procedure: noExist at line: 5")
            );
            REQUIRE(parse(problemCode2) == nullptr);
        }

        SECTION("Simple Cyclical call should not be allowed") {
            std::string problemCode = R"(
                procedure a {
                    call b;
                }
                procedure b {
                    call c;
                }
                procedure c {
                    call a;
                }
            )";
            auto tokens = Lexer(problemCode).getTokens();
            // It will catch the call for non-existent procedure "noExist" at the 5th line of the source code
            REQUIRE_THROWS_MATCHES(
                parseProgram(tokens),
                std::invalid_argument,
                Catch::Message("Cyclical call detected")
            );
            REQUIRE(parse(problemCode) == nullptr);
        }

        SECTION("Recursive call should not be allowed") {
            std::string problemCode = R"(
                procedure a {
                    call a;
                }
            )";
            auto tokens = Lexer(problemCode).getTokens();
            // It will catch the call for non-existent procedure "noExist" at the 5th line of the source code
            REQUIRE_THROWS_MATCHES(
                parseProgram(tokens),
                std::invalid_argument,
                Catch::Message("Cyclical call detected")
            );
            REQUIRE(parse(problemCode) == nullptr);
        }

        SECTION("Cyclical call in disjoint graph should not be allowed") {
            std::string problemCode = R"(
                procedure a {
                    call b;
                }
                procedure b {
                    read x;
                }
                procedure c {
                    read x;
                }
                procedure d {
                    call d;
                }
            )";
            auto tokens = Lexer(problemCode).getTokens();
            // It will catch the call for non-existent procedure "noExist" at the 5th line of the source code
            REQUIRE_THROWS_MATCHES(
                parseProgram(tokens),
                std::invalid_argument,
                Catch::Message("Cyclical call detected")
            );
            REQUIRE(parse(problemCode) == nullptr);
        }
    }
}
}  // namespace parser
}  // namespace sp
