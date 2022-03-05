#include <iostream>
#include <vector>

#include "catch.hpp"
#include "Parser/AST.h"

void require(bool b) {
    REQUIRE(b);
}

namespace sp {
namespace ast {
TEST_CASE("AST Test") {
    // Construction test
    INFO("Constructing the AST");

    auto v1 = std::make_unique<Var>("v1");
    auto v2 = std::make_unique<Var>("v2");
    auto v3 = std::make_unique<Var>("v3");
    auto v4 = std::make_unique<Var>("v4");
    auto v5 = std::make_unique<Var>("v5");
    auto v6 = std::make_unique<Var>("v6");

    auto one = std::make_unique<Const>(1);
    auto constVal10 = std::make_unique<Const>(10);
    auto constVal5 = std::make_unique<Const>(5);

    SECTION("Assign construction") {
        auto assign = std::make_unique<Assign>(1, std::move(v1), std::move(v2));  // v1 = v2
    };

    SECTION("Binary Expression construction") {
        // v1 + v2
        auto binExpr1 = std::make_unique<ast::BinExpr>(ast::BinOp::PLUS, std::move(v1), std::move(v2));
        // 10 - 5
        auto binExpr2 = std::make_unique<ast::BinExpr>(ast::BinOp::MINUS, std::move(constVal10), std::move(constVal5));
        // (v1 + v2) + (10 - 5)
        auto binExpr3 = std::make_unique<ast::BinExpr>(ast::BinOp::PLUS, std::move(binExpr1), std::move(binExpr2));
    };

    SECTION("Condition Expression construction") {
        // 10 > 5
        auto relExpr1 = std::make_unique<RelExpr>(RelOp::GT, std::move(constVal10), std::move(constVal5));
        // v1 == v2
        auto relExpr2 = std::make_unique<RelExpr>(RelOp::EQ, std::move(v1), std::move(v2));
        // (10 > 5) && (v1 == v2)
        auto condExpr = std::make_unique<CondBinExpr>(CondOp::AND, std::move(relExpr1), std::move(relExpr2));
    };

    SECTION("IO Statements Construction") {
        // read
        auto readStmt = std::make_unique<Read>(20, std::move(v1));
        auto readStmt2 = std::make_unique<Read>(69, std::move(v2));
        // print
        auto printStmt = std::make_unique<Print>(21, std::move(v3));
    }

    SECTION("Statement Construction") {
        auto readStmt = std::make_unique<Read>(20, std::move(v1));
        auto readStmt2 = std::make_unique<Read>(69, std::move(v2));
        auto printStmt = std::make_unique<Print>(21, std::move(v3));

        // while ((10 > 5) && (v1 == v2)) {
        //     read v1;
        //     print v3;
        // }
        auto relExpr1 = std::make_unique<RelExpr>(RelOp::GT, std::move(constVal10), std::move(constVal5));
        auto relExpr2 = std::make_unique<RelExpr>(RelOp::EQ, std::move(v4), std::move(v5));
        auto condExpr = std::make_unique<CondBinExpr>(CondOp::AND, std::move(relExpr1), std::move(relExpr2));
        std::vector<std::unique_ptr<Statement>> vect;
        vect.push_back(std::move(readStmt));
        vect.push_back(std::move(printStmt));
        StmtLst lst(vect);

        auto whileStmt = std::make_unique<While>(22, std::move(condExpr), std::move(lst));
        // if ((10 > 5) && (v1 == v2)) then {
        //     read v2;
        // } else {
        // }
        std::vector<std::unique_ptr<Statement>>  vect2;
        vect2.push_back(std::move(readStmt2));
        StmtLst lst2(vect2);
        std::vector<std::unique_ptr<Statement>> vect3;
        StmtLst lst3(vect3);
        auto ifStmt = std::make_unique<If>(80, std::move(condExpr), std::move(lst2), std::move(lst3));
        // assign
        auto assignStmt = std::make_unique<Assign>(42, std::move(v6), std::make_unique<Const>(69));
    }

    SECTION("Equality Check") {
        // Var
        auto var = make<Var>("abdc");
        auto var2 = make<Var>("abd");
        auto var3 = make<Var>("abdc");
        REQUIRE(*var == *var);
        REQUIRE(*var == *var3);
        REQUIRE_FALSE(*var == *var2);

        // Const
        auto const1 = make<Const>(69);
        auto const2 = make<Const>(420);
        auto const3 = make<Const>(69);
        REQUIRE(*const1 == *const1);
        REQUIRE(*const1 == *const3);
        REQUIRE_FALSE(*const1 == *const2);

        // BinOp
        auto binOp = make<BinExpr>(BinOp::PLUS, make<Const>(1), make<Var>("x"));
        auto binOp1 = make<BinExpr>(BinOp::MINUS, make<Const>(1), make<Var>("x"));
        auto binOp2 = make<BinExpr>(BinOp::PLUS, make<Const>(1), make<Var>("y"));
        auto binOp3 = make<BinExpr>(BinOp::PLUS, make<Const>(1), make<Var>("x"));
        REQUIRE(*binOp == *binOp);
        REQUIRE(*binOp == *binOp3);
        REQUIRE_FALSE(*binOp == *binOp1);
        REQUIRE_FALSE(*binOp == *binOp2);

        // Read
        auto readStmt = make<Read>(1, make<Var>("aaa"));
        auto readStmt1 = make<Read>(1, make<Var>("bbb"));
        auto readStmt2 = make<Read>(2, make<Var>("aaa"));
        auto readStmt3 = make<Read>(1, make<Var>("aaa"));
        REQUIRE(*readStmt == *readStmt);
        REQUIRE(*readStmt == *readStmt3);
        REQUIRE_FALSE(*readStmt == *readStmt1);
        REQUIRE_FALSE(*readStmt == *readStmt2);

        // Print
        auto printStmt = make<Print>(1, make<Var>("aaa"));
        auto printStmt1 = make<Print>(1, make<Var>("bbb"));
        auto printStmt2 = make<Print>(2, make<Var>("aaa"));
        auto printStmt3 = make<Print>(1, make<Var>("aaa"));
        REQUIRE(*printStmt == *printStmt);
        REQUIRE(*printStmt == *printStmt3);
        REQUIRE_FALSE(*printStmt == *printStmt1);
        REQUIRE_FALSE(*printStmt == *printStmt2);

        // BinExpr
        auto binExpr = make<BinExpr>(BinOp::MINUS, make<Var>("x"), make<Var>("y"));
        auto binExpr1 = make<BinExpr>(BinOp::DIVIDE, make<Var>("x"), make<Var>("y"));
        auto binExpr2 = make<BinExpr>(BinOp::MINUS, make<Var>("x"), make<Const>(69));
        auto binExpr3 = make<BinExpr>(BinOp::MINUS, make<Var>("x"), make<Var>("y"));
        REQUIRE(*binExpr == *binExpr);
        REQUIRE(*binExpr == *binExpr3);
        REQUIRE_FALSE(*binExpr == *binExpr1);
        REQUIRE_FALSE(*binExpr == *binExpr2);

        // Assign
        auto assnStmt = make<Assign>(1, 
            make<Var>("aaa"), make<BinExpr>(BinOp::MINUS, make<Var>("x"), make<Var>("y")));
        auto assnStmt1 = make<Assign>(1, 
            make<Var>("bbb"), make<BinExpr>(BinOp::MINUS, make<Var>("x"), make<Const>(69)));
        auto assnStmt2 = make<Assign>(2, 
            make<Var>("aaa"), make<BinExpr>(BinOp::MINUS, make<Var>("x"), make<Var>("y")));
        auto assnStmt3 = make<Assign>(1, 
            make<Var>("aaa"), make<BinExpr>(BinOp::MINUS, make<Var>("x"), make<Var>("y")));
        REQUIRE(*assnStmt == *assnStmt);
        REQUIRE(*assnStmt == *assnStmt3);
        REQUIRE_FALSE(*assnStmt == *assnStmt1);
        REQUIRE_FALSE(*assnStmt == *assnStmt2);

        // StmtLst
        auto stmtLst = makeStmts(
            make<Read>(2, make<Var>("y")),
            make<Print>(3, make<Var>("x")),
            make<Assign>(4, make<Var>("y"), make<BinExpr>(BinOp::MINUS, make<Var>("x"), make<Var>("y")))
        );
        auto stmtLst1 = makeStmts(
            make<Assign>(2, make<Var>("y"), make<BinExpr>(BinOp::MINUS, make<Var>("x"), make<Var>("y"))),
            make<Print>(3, make<Var>("x")),
            make<Read>(4, make<Var>("y"))
        );
        auto stmtLst2 = makeStmts(
            make<Read>(2, make<Var>("y")),
            make<Assign>(4, make<Var>("y"), make<BinExpr>(BinOp::MINUS, make<Var>("x"), make<Var>("y")))
        );
        auto stmtLst3 = makeStmts(
            make<Read>(2, make<Var>("y")),
            make<Print>(3, make<Var>("x")),
            make<Assign>(4, make<Var>("y"), make<BinExpr>(BinOp::MINUS, make<Var>("x"), make<Var>("y")))
        );
        REQUIRE(stmtLst == stmtLst);
        REQUIRE(stmtLst == stmtLst3);
        REQUIRE_FALSE(stmtLst == stmtLst1);
        REQUIRE_FALSE(stmtLst == stmtLst2);

        // RelExpr
        auto relExpr = make<RelExpr>(RelOp::GT, make<Var>("x"), make<Const>(10));
        auto relExpr1 = make<RelExpr>(RelOp::LT, make<Var>("x"), make<Const>(10));
        auto relExpr2 = make<RelExpr>(RelOp::GT, make<Var>("x"), make<Var>("y"));
        auto relExpr3 = make<RelExpr>(RelOp::GT, make<Var>("x"), make<Const>(10));
        REQUIRE(*relExpr == *relExpr);
        REQUIRE(*relExpr == *relExpr3);
        REQUIRE_FALSE(*relExpr == *relExpr1);
        REQUIRE_FALSE(*relExpr == *relExpr2);


        // NotCondExpr
        auto notCondExpr = std::make_unique<NotCondExpr>(make<RelExpr>(RelOp::GT, make<Var>("x"), make<Const>(10)));
        auto notCondExpr1 = std::make_unique<NotCondExpr>(make<RelExpr>(RelOp::LT, make<Var>("x"), make<Const>(10)));
        auto notCondExpr2 = std::make_unique<NotCondExpr>(make<RelExpr>(RelOp::GT, make<Var>("x"), make<Var>("y")));
        auto notCondExpr3 = std::make_unique<NotCondExpr>(make<RelExpr>(RelOp::GT, make<Var>("x"), make<Const>(10)));
        REQUIRE(*notCondExpr == *notCondExpr);
        REQUIRE(*notCondExpr == *notCondExpr3);
        REQUIRE_FALSE(*notCondExpr == *notCondExpr1);
        REQUIRE_FALSE(*notCondExpr == *notCondExpr2);

        // CondBinExpr
        auto condBinExpr = make<CondBinExpr>(CondOp::AND,
            make<RelExpr>(RelOp::GT, make<Var>("x"), make<Const>(10)),
            make<RelExpr>(RelOp::LT, make<Var>("y"), make<Const>(69)));
        auto condBinExpr1 = make<CondBinExpr>(CondOp::AND,
            make<RelExpr>(RelOp::LT, make<Var>("y"), make<Const>(69)),
            make<RelExpr>(RelOp::GT, make<Var>("x"), make<Const>(10)));
        auto condBinExpr2 = make<CondBinExpr>(CondOp::OR,
            make<RelExpr>(RelOp::GT, make<Var>("x"), make<Const>(10)),
            make<RelExpr>(RelOp::LT, make<Var>("y"), make<Const>(69)));
        auto condBinExpr3 = make<CondBinExpr>(CondOp::AND,
            make<RelExpr>(RelOp::GT, make<Var>("x"), make<Const>(10)),
            make<RelExpr>(RelOp::LT, make<Var>("y"), make<Const>(69)));
        REQUIRE(*condBinExpr == *condBinExpr);
        REQUIRE(*condBinExpr == *condBinExpr3);
        REQUIRE_FALSE(*condBinExpr == *condBinExpr1);
        REQUIRE_FALSE(*condBinExpr == *condBinExpr2);

        auto genSampleCondExpr = [](CondOp op) {
            return make<CondBinExpr>(
                op,
                make<RelExpr>(RelOp::GT, make<Var>("x"), make<Const>(10)),
                make<RelExpr>(RelOp::LT, make<Var>("y"), make<Const>(69)));
        };
        auto genStmtLst = []() {
            return makeStmts(
                make<Read>(2, make<Var>("y")),
                make<Print>(3, make<Var>("x")),
                make<Assign>(4, make<Var>("y"), make<BinExpr>(BinOp::MINUS, make<Var>("x"), make<Var>("y")))
            );
        };

        // WhileStmt
        auto whileStmt1 = make<While>(1, genSampleCondExpr(CondOp::AND), genStmtLst());
        auto whileStmt2 = make<While>(2, genSampleCondExpr(CondOp::AND), genStmtLst());
        auto whileStmt3 = make<While>(1, genSampleCondExpr(CondOp::OR), genStmtLst());
        auto whileStmt4 = make<While>(1, genSampleCondExpr(CondOp::AND), genStmtLst());
        REQUIRE(*whileStmt1 == *whileStmt1);
        REQUIRE(*whileStmt1 == *whileStmt4);
        REQUIRE_FALSE(*whileStmt1 == *whileStmt2);
        REQUIRE_FALSE(*whileStmt1 == *whileStmt3);

        /**
         * if (x > 10) then {
         *      read y;
         *      print x;
         *      y = x - y;
         * } else {
         *      read z;
         *      print v;
         *      v = z - x;
         * }
         */
        auto ifThenElse = make<If>(1, make<RelExpr>(RelOp::GT, make<Var>("x"), make<Const>(10)),
            makeStmts(
                make<Read>(2, make<Var>("y")),
                make<Print>(3, make<Var>("x")),
                make<Assign>(4, make<Var>("y"), make<BinExpr>(BinOp::MINUS, make<Var>("x"), make<Var>("y")))
            ),
            makeStmts(
                make<Read>(5, make<Var>("z")),
                make<Print>(6, make<Var>("v")),
                make<Assign>(7, make<Var>("v"), make<BinExpr>(BinOp::MINUS, make<Var>("z"), make<Var>("x")))
            ));
        auto ifThenElse1 = make<If>(1, make<RelExpr>(RelOp::GT, make<Var>("x"), make<Const>(10)),
            makeStmts(
                make<Read>(2, make<Var>("x")),
                make<Print>(3, make<Var>("y")),
                make<Assign>(4, make<Var>("x"), make<BinExpr>(BinOp::MINUS, make<Var>("x"), make<Var>("y")))
            ),
            makeStmts(
                make<Read>(5, make<Var>("z")),
                make<Print>(6, make<Var>("v")),
                make<Assign>(7, make<Var>("v"), make<BinExpr>(BinOp::MINUS, make<Var>("z"), make<Var>("x")))
            ));
        auto ifThenElse2 = make<If>(1, make<RelExpr>(RelOp::GT, make<Var>("x"), make<Const>(10)),
            makeStmts(
                make<Read>(2, make<Var>("y")),
                make<Print>(3, make<Var>("x")),
                make<Assign>(4, make<Var>("y"), make<BinExpr>(BinOp::MINUS, make<Var>("x"), make<Var>("y")))
            ),
            makeStmts(
                make<Read>(5, make<Var>("z")),
                make<Print>(6, make<Var>("v")),
                make<Assign>(7, make<Var>("v"), make<BinExpr>(BinOp::MINUS, make<Var>("z"), make<Var>("x")))
            ));
        REQUIRE(*ifThenElse == *ifThenElse);
        REQUIRE(*ifThenElse == *ifThenElse2);
        REQUIRE_FALSE(*ifThenElse == *ifThenElse1);
        
        // Procedure
        auto proc1 = make<Procedure>("monkey", genStmtLst());
        auto proc2 = make<Procedure>("ooga", genStmtLst());
        auto proc3 = make<Procedure>("monkey", genStmtLst());
        REQUIRE(*proc1 == *proc1);
        REQUIRE(*proc1 == *proc3);
        REQUIRE_FALSE(*proc1 == *proc2);

        // Program
        auto prog1 = std::make_unique<Program>(make<Procedure>("monkey", genStmtLst()));
        auto prog2 = std::make_unique<Program>(make<Procedure>("ooga", genStmtLst()));
        auto prog3 = std::make_unique<Program>(make<Procedure>("monkey", genStmtLst()));
        REQUIRE(prog1 == prog1);
        REQUIRE(*prog1 == *prog3);
        REQUIRE_FALSE(prog1 == prog2);
    }

    SUCCEED("No errors thrown");
}
}  // namespace ast
}  // namespace sp
