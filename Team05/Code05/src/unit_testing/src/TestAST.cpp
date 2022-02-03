#include <iostream>
#include <vector>

#include "catch.hpp"
#include "Parser/AST.h"

void require(bool b) {
    REQUIRE(b);
}


namespace AST {
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
        auto binExpr1 = std::make_unique<AST::BinExpr>(AST::BinOp::PLUS, std::move(v1), std::move(v2));
        // 10 - 5
        auto binExpr2 = std::make_unique<AST::BinExpr>(AST::BinOp::MINUS, std::move(constVal10), std::move(constVal5));
        // (v1 + v2) + (10 - 5)
        auto binExpr3 = std::make_unique<AST::BinExpr>(AST::BinOp::PLUS, std::move(binExpr1), std::move(binExpr2));
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

    SECTION("Factory Construction") {
        auto var = make<Var>("abdc");
        auto readmake = make<Read>(1, make<Var>("aaa"));
        auto binOp = make<BinExpr>(BinOp::PLUS, make<Const>(1), make<Var>("x"));
        auto stmt = makeStmts(make<Read>(1, make<Var>("aaa")), make<Print>(2, make<Var>("aaa")));


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

    }

    SUCCEED("No errors thrown during object construction");
}
}  // namespace AST

