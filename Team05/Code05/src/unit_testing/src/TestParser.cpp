
#include <iostream>

#include "catch.hpp"
#include "Parser/AST.h"
#include "Parser/Lexer.h"
#include "Parser/Parser.h"


class ParserUnitTest {
public:
    void test() {
        std::deque<Token> tokens;
        using AST::make;
        std::unique_ptr<AST::ASTNode> ast, expected;
        SECTION("Parser().parseConstExpr") {
            tokens = std::deque<Token>({ Token{TokenType::number, 5} });
            ast = Parser().parseConstExpr(tokens);
            expected = make<AST::Const>(5);
            REQUIRE(*ast == *expected);
        }  // Const
        SECTION("Parser().parseVariableExpr") {
            tokens = std::deque<Token>({ Token{TokenType::name, "Variable11"} });
            ast = Parser().parseVariableExpr(tokens);
            expected = make<AST::Var>("Variable11");
            REQUIRE(*ast == *expected);
        };
        SECTION("Parser().parseRelOp") {
            tokens = std::deque<Token>({
                Token{TokenType::special, '='},
                Token{TokenType::special, '>'}
                });
            REQUIRE_THROWS(Parser().parseRelOp(tokens));
            tokens = std::deque<Token>({
                Token{TokenType::special, '='},
                Token{TokenType::special, '!'}
                });
            REQUIRE_THROWS(Parser().parseRelOp(tokens));
            tokens = std::deque<Token>({
                Token{TokenType::special, '!'},
                Token{TokenType::eof}
                });
            REQUIRE_THROWS(Parser().parseRelOp(tokens));
            tokens = std::deque<Token>({
                Token{TokenType::special, '='},
                Token{TokenType::eof}
                });
            REQUIRE_THROWS(Parser().parseRelOp(tokens));


            tokens = std::deque<Token>({
                Token{TokenType::special, '>'},
                Token{TokenType::special, '='}
                });
            REQUIRE(Parser().parseRelOp(tokens) == AST::RelOp::GTE);
            tokens = std::deque<Token>({
                Token{TokenType::special, '!'},
                Token{TokenType::special, '='}
                });
            REQUIRE(Parser().parseRelOp(tokens) == AST::RelOp::NE);
            tokens = std::deque<Token>({
                Token{TokenType::special, '<'},
                Token{TokenType::special, '='}
                });
            REQUIRE(Parser().parseRelOp(tokens) == AST::RelOp::LTE);
            tokens = std::deque<Token>({
                Token{TokenType::special, '='},
                Token{TokenType::special, '='}
                });
            REQUIRE(Parser().parseRelOp(tokens) == AST::RelOp::EQ);
            tokens = std::deque<Token>({
                Token{TokenType::special, '>'},
                Token{TokenType::eof}
                });
            REQUIRE(Parser().parseRelOp(tokens) == AST::RelOp::GT);
            tokens = std::deque<Token>({
                Token{TokenType::special, '<'},
                Token{TokenType::eof}
                });
            REQUIRE(Parser().parseRelOp(tokens) == AST::RelOp::LT);
        }  // RelOp
        SECTION("Parser().parseCondOp") {
            tokens = std::deque<Token>({
                Token{TokenType::special, '&'},
                Token{TokenType::special, '|'}
                });
            REQUIRE_THROWS(Parser().parseCondOp(tokens));
            tokens = std::deque<Token>({
                Token{TokenType::special, '|'},
                Token{TokenType::special, '&'}
                });
            REQUIRE_THROWS(Parser().parseCondOp(tokens));
            tokens = std::deque<Token>({
                Token{TokenType::special, '&'},
                Token{TokenType::special, '!'}
                });
            REQUIRE_THROWS(Parser().parseCondOp(tokens));
            tokens = std::deque<Token>({
                Token{TokenType::special, '|'},
                Token{TokenType::special, '!'}
                });
            REQUIRE_THROWS(Parser().parseCondOp(tokens));


            tokens = std::deque<Token>({
                Token{TokenType::special, '&'},
                Token{TokenType::special, '&'}
                });
            REQUIRE(Parser().parseCondOp(tokens) == AST::CondOp::AND);
            tokens = std::deque<Token>({
                Token{TokenType::special, '|'},
                Token{TokenType::special, '|'}
                });
            REQUIRE(Parser().parseCondOp(tokens) == AST::CondOp::OR);
        }  // CondOp
        SECTION("Parser().parseBinOp") {
            tokens = std::deque<Token>({
                Token{TokenType::special, '+'}
                });
            REQUIRE(Parser().parseBinOp(tokens) == AST::BinOp::PLUS);
            tokens = std::deque<Token>({
                Token{TokenType::special, '-'}
                });
            REQUIRE(Parser().parseBinOp(tokens) == AST::BinOp::MINUS);
            tokens = std::deque<Token>({
                Token{TokenType::special, '/'}
                });
            REQUIRE(Parser().parseBinOp(tokens) == AST::BinOp::DIVIDE);
            tokens = std::deque<Token>({
                Token{TokenType::special, '*'}
                });
            REQUIRE(Parser().parseBinOp(tokens) == AST::BinOp::MULT);
            tokens = std::deque<Token>({
                Token{TokenType::special, '%'}
                });
            REQUIRE(Parser().parseBinOp(tokens) == AST::BinOp::MOD);
        }
        SECTION("Parser().shuntingYardParser") {
            tokens = Lexer("4+x*2/(1-y)-6%8").getTokens();
            ast = Parser().shuntingYardParser(tokens);
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
            auto ast1 = Parser().shuntingYardParser(tokens);
            tokens = Lexer("(4+3)+2").getTokens();
            auto ast2 = Parser().shuntingYardParser(tokens);
            tokens = Lexer("4+(3+2)").getTokens();
            auto ast3 = Parser().shuntingYardParser(tokens);
            REQUIRE(*ast1 == *ast2);
            REQUIRE(!(*ast1 == *ast3));
        }
        SECTION("Parser().parseRelExpr") {
            tokens = Lexer("1 > 2").getTokens();
            ast = Parser().parseRelExpr(tokens);
            expected = make<AST::RelExpr>(
                AST::RelOp::GT,
                make<AST::Const>(1),
                make<AST::Const>(2)
                );
            REQUIRE(*ast == *expected);

            tokens = Lexer("x > y").getTokens();
            ast = Parser().parseRelExpr(tokens);
            expected = make<AST::RelExpr>(
                AST::RelOp::GT,
                make<AST::Var>("x"),
                make<AST::Var>("y")
                );
            REQUIRE(*ast == *expected);

            tokens = Lexer("(x+1) > (y+2)").getTokens();
            ast = Parser().parseRelExpr(tokens);
            expected = make<AST::RelExpr>(
                AST::RelOp::GT,
                make<AST::BinExpr>(AST::BinOp::PLUS, make<AST::Var>("x"), make<AST::Const>(1)),
                make<AST::BinExpr>(AST::BinOp::PLUS, make<AST::Var>("y"), make<AST::Const>(2))
                );
            REQUIRE(*ast == *expected);
        }
        SECTION("Parser().parseCondExpr") {
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
            std::unique_ptr<AST::CondExpr> ast = Parser().parseCondExpr(tokens);
            std::unique_ptr<AST::CondExpr> expected = make<AST::CondBinExpr>(
                AST::CondOp::AND,
                relExpr1(),
                relExpr2()
                );
            REQUIRE(*ast == *expected);

            tokens = Lexer("((x+1)>(y+2))||((z+1)>(t+2))").getTokens();
            ast = Parser().parseCondExpr(tokens);
            expected = make<AST::CondBinExpr>(
                AST::CondOp::OR,
                relExpr1(),
                relExpr2()
                );
            REQUIRE(*ast == *expected);

            tokens = Lexer("!((x+1)>(y+2))").getTokens();
            ast = Parser().parseCondExpr(tokens);
            expected = std::make_unique<AST::NotCondExpr>(
                relExpr1()
                );
            REQUIRE(*ast == *expected);
        }
        SECTION("Parser().parseReadStmt") {
            tokens = Lexer("read v1;").getTokens();
            ast = Parser().parseReadStmt(tokens);
            expected = make<AST::Read>(1, make<AST::Var>("v1"));
            REQUIRE(*ast == *expected);

            tokens = Lexer("read 1v;").getTokens();
            REQUIRE_THROWS(Parser().parseReadStmt(tokens));
        }
        SECTION("Parser().parsePrintStmt") {
            tokens = Lexer("print v1;").getTokens();
            ast = Parser().parsePrintStmt(tokens);
            expected = make<AST::Print>(1, make<AST::Var>("v1"));
            REQUIRE(*ast == *expected);

            tokens = Lexer("read 1v;").getTokens();
            REQUIRE_THROWS(Parser().parsePrintStmt(tokens));
        }
        SECTION("Parser().parseAssignStmt") {
            tokens = Lexer("x = y;").getTokens();
            ast = Parser().parseAssignStmt(tokens);
            expected = make<AST::Assign>(
                1,
                make<AST::Var>("x"),
                make<AST::Var>("y")
            );
            REQUIRE(*ast == *expected);

            tokens = Lexer("x = y + 1;").getTokens();
            ast = Parser().parseAssignStmt(tokens);
            expected = make<AST::Assign>(
                1,
                make<AST::Var>("x"),
                make<AST::BinExpr>(AST::BinOp::PLUS, make<AST::Var>("y"), make<AST::Const>(1))
            );
            REQUIRE(*ast == *expected);

            tokens = Lexer("2 = y + 1;").getTokens();
            REQUIRE_THROWS(Parser().parseAssignStmt(tokens));
        }
        SECTION("Parser().parseStmtLst") {
            tokens = Lexer(R"(
                read v1;
                print v1;
                v2 = v1;
            )").getTokens();

            auto stmtlst = Parser().parseStmtLst(tokens);
            auto expectedStmtlst = AST::makeStmts(
                make<AST::Read>(1, make<AST::Var>("v1")),
                make<AST::Print>(2, make<AST::Var>("v1")),
                make<AST::Assign>(3, make<AST::Var>("v2"), make<AST::Var>("v1"))
            );
            REQUIRE(stmtlst == expectedStmtlst);
        }
        SECTION("Parser().parseWhileStmt") {
            tokens = Lexer(R"(
            while (x < 10) {
                read v1;
                print v1;
                v2 = v1;
            }
            )").getTokens();

            ast = Parser().parseWhileStmt(tokens);
            auto genStmtlst = []() {
                return AST::makeStmts(
                    make<AST::Read>(2, make<AST::Var>("v1")),
                    make<AST::Print>(3, make<AST::Var>("v1")),
                    make<AST::Assign>(4, make<AST::Var>("v2"), make<AST::Var>("v1"))
                );
            };
            expected = make<AST::While>(1,
                make<AST::RelExpr>(AST::RelOp::LT, make<AST::Var>("x"), make <AST::Const>(10)),
                genStmtlst()
            );

            REQUIRE(*ast == *expected);
        }
        SECTION("Parser().parseIfStmt") {
            tokens = Lexer(R"(
            if (x < 10) then {
                read v1;
                print v1;
                v2 = v1;
            } else {
                read v1;
                print v1;
                v2 = v1;
            }
            )").getTokens();
            ast = Parser().parseIfStmt(tokens);
            auto genStmtlst = [](int i) {
                return AST::makeStmts(
                    make<AST::Read>(i+1, make<AST::Var>("v1")),
                    make<AST::Print>(i+2, make<AST::Var>("v1")),
                    make<AST::Assign>(i+3, make<AST::Var>("v2"), make<AST::Var>("v1"))
                );
            };
            expected = make<AST::If>(1,
                make<AST::RelExpr>(AST::RelOp::LT, make<AST::Var>("x"), make <AST::Const>(10)),
                genStmtlst(1),
                genStmtlst(4)
            );
            REQUIRE(*ast == *expected);
        }
        SECTION("Parser().parseProcedure") {
            tokens = Lexer(R"(
            procedure main {
                read v1;
                print v1;
                v2 = v1;
            }
            )").getTokens();

            ast = Parser().parseProcedure(tokens);
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
        SECTION("Parser().parseProgram") {
            tokens = Lexer(R"(
            procedure main {
                read v1;
                print v1;
                v2 = v1;
            }
            )").getTokens();

            ast = Parser().parseProgram(tokens);

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
};

TEST_CASE("Testing Parser") {
    SECTION("Unit testing") {
        ParserUnitTest().test();
    }

    SECTION("Complete procedure test") {
        std::string fail = R"(procedure a {
            while (c !! b) {
                read c; 
            }
        })";
        REQUIRE(Parser().parse(fail) == nullptr);

        std::string fail2 = R"(procedure a {
            while (!c)) {
                read c;
            }
        })";
        REQUIRE(Parser().parse(fail2) == nullptr);


        std::string fail3 = R"(procedure 1a {
            1x = 1y + 1z;
        })";
        REQUIRE(Parser().parse(fail3) == nullptr);

        std::string testCode = R"(procedure computeAverage {
            read num1;
            read num2;
            read num3;
            sum = num1 + num2 + num3;
            ave = sum / 3;
            print ave;
        })";
        REQUIRE(Parser().parse(testCode) != nullptr);


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
        REQUIRE(Parser().parse(testCode2) != nullptr);

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
        REQUIRE(Parser().parse(generatedCode1) != nullptr);

        std::string generatedCode2 = R"(procedure h7JOVeUY {
 JqVi =  72 % (556 - ix70);
if (nxg9F5 + 0 * (JCq81 - 04 % (1 - 0)) == 180) then {
p1hb3wd =  (0 + (oEX8A) - A000 / (((A000 + 149 / 0513)) + 2033 / (aCtu4) % (364 % (833 - 730 - 0 % (72 % 0) - 0 % 0) % 4) % (Ao5eB) * (0 / 436) - 0) - 014 * A000) + qZZD72Av * (0 / (2608 - (0))) % (A000) % (7531 - UzRG);
if (09 / 0 <= (5 - A000) * (z6665rVc + (0 + zPQUJjo)) - 06 % (0 + 0529)) then {
read   A000;
print  ENtf2K;
} else {
print  nkqQ76LT;
JxkH =  (5 + 65) % (SZXKh % 843 * 44);
}
if ((8 + 0 == 0 / (70) * (4568)) || (!((345 * 0 < 475) && (!(71 != 02))))) then {
if (16 >= 4) then {
A000 =  o2r6z2V - y7P10 / 8670 + lezAUYbA;
} else {
fv732 =   b4ke44 - 9;
}
} else {
PYg9aR =  (0) * (5789 * 9) / (Se2K - 5 - 3) * (173 - (9)) / 290;
}
} else {
if (((!(!(!(!(5 >= 0))))) || (((EZmB3HfN != vQLg0z) || (!(0 >= 0))) && (6891 != 3 / (1877 / (0))))) && (hVtzf - (84 % 8686 - (04)) <= c3M3ayR + 0 % 740 - 5159 / 621)) then {
print  pf9e8RW;
while    (!(!(!(86 <= A000))))   {
read  hd7H75h;
}
ENmc =   0;
} else {
read  A000;
read  t7PlWOif;
}
scjN5q =  6224 / 0 + Ahf44k - (0 + 374 + 0 % 0) - 0 % 06 + (5 / (A000) * (4805 + BcxhtDU - 182 % (MKxgepP + A000) - 0788 / 395) * (01 + E290 / 7776 - 277 % 0 + 4 / 52 + A000)) % uJNLVtn - 0 * (8925 - 0 * SAQ6wW - (A000) % A000 + (0 + (8817)));
}
while    ((Ronwi8tu * 0 * 4614 % HxxD7cj7) - ((yq7Nd5SD + 0 + 799 - z4j18 / 8645)) / 5 - 0 % 347 * (33 / (0 + 85 % 6) + A000 * Gp1vKu) - (cyBcGKza / (5) / (884 - 25) / (0235 % Hfp7zn5)) / ((0) / 0) / ((6) / 439 % 0 * (6 % 6 + Wo181) + 70 / (026 - ((M63EY + 1269 % (JzGmb)) / (7376 / 88 - 751) / (077 - 4931 / 5 - (A000) / (0 % 74)))) * ((88 - O01Ev8) - 0 % 0) / (r5or)) == 0 / 0 % c3VcND5 * (ehoE) % 0 - ((424) - Z58F) % (745 % j16A4zqm - 0 / 28) / (0 / A000 * (k6dJ % 8 + (2260 + 92 - tSD2Mfd)) * (((5 * (0931 + 4))))))   {
Oufw7V =   (3563 - 269 / 59 + qjZ4 * (hXnq + 0127) / (0 + 69 - (3073 * (0) - (921)) + 0) - (vrf9HtbB + A000 - 96 * 2681 + (n00z55 % 3 * 32 + 0) - (YCneamC0 * (9783 / 0 % (166) - 5) % (N2XBT % xURP9O % (33) * ((0 + 16 * 6 - (44) + (0 / 35))))) * (0 % 4 % (55 - GnmdK2Xz) - 1988 % (N111hwg - (49 % (dphmA71 - A000 - W2Xt4N * tk4K4nd6))) % (073 + p5X24st - R9yoZHXX / 0077 - (0 % (82 * 17) / 0 % E14I76L))))) % ((143) / (06 - 0) * (SKi3KiQp + 393) * (88 / 0 / pNE2a10 - (BXB932q) % OBwnsAl * JyfD3ec + (458 % (9981 % (0)) - h9j2 * 317)) / (aZ1z - 0 + 471 % (W0Rf / (0) - Ham9o3 / (797 + (2899) / 7392 - 53 / (909) * 0) / (261 / 0 - 0 * A000 % e12ssAI)) + 33 % (1145 / 551) + 205 / 440 % (2) - (E2I1dBr % (6))) - 0 / ((0 - 25 / 660 - 0 % M9ZlWi % A000 / 1 + (9) % 34 * (0) % (0 / (0 + 6) % (12 + (0 * ((0 / 20))) * (0) / 429) - (cJK41v8A) - 9890 / D00X % (7) % (436 * 458)))));
}
BpZjf =   (0 - 3241) / (xS8Y5myR % (TW0P)) + (7753) % (5582 + A000 - e5k575j4 % 0 - ((006 * a0k1) + qbmdQh - 2));
BPs82 =  (2) % (263 - 9374) * ((814) + (0 + o9y0Nxi) % A000 - 0 / 035) % (53 - 272 % 7 - 801 * A000) * (03 % (lGUy - 59));

})";
        REQUIRE(Parser().parse(generatedCode2) != nullptr);
    }    
}
