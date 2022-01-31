#include <iostream>
#include <vector>
#include <cstdarg>
#include <stdio.h>

#include "catch.hpp"
#include "Parser/AST.h"
#include "DesignExtractor.h"
#include "logging.h"


namespace AST {
    using std::move;

    // for making Var and const
    template <typename T, typename K>
    std::unique_ptr<T> makeBasic(K v) {
        return std::make_unique<T>(v);
    }

    std::unique_ptr<Read> makeRead(int num, std::string name) {
        return std::make_unique<Read>(num, makeBasic<Var>(name));
    }

    std::unique_ptr<Print> makePrint(int num, std::string name) {
        return std::make_unique<Print>(num, makeBasic<Var>(name));
    };

    std::unique_ptr<Assign> makeAssign(int num, std::string lhs, std::unique_ptr<Expr> rhs) {
        return std::make_unique<Assign>(num, makeBasic<Var>(lhs), std::move(rhs));
    }


    StmtLst makeStatementLst(std::vector<std::unique_ptr<Statement>> vect) {
        return StmtLst(std::move(vect));
    }

    template <typename T, typename K>
    std::unique_ptr<RelExpr> makeRelExpr(RelOp op, T LHS, K RHS) {
        return std::make_unique<RelExpr>(op, std::make_unique<T>(LHS), std::make_unique<K>(RHS));
    };

    template <typename T, typename K>
    std::unique_ptr<Assign> makeAssign(int num, T LHS, K RHS) {
        return std::make_unique<Assign>(num, std::make_unique<T>(LHS), std::move(RHS));
    };

    template <typename T, typename K>
    std::unique_ptr<BinExpr> makeBinExpr(BinOp op, T LHS, K RHS) {
        return std::make_unique<BinExpr>(op, std::make_unique<T>(LHS), std::make_unique<K>(RHS));
    };

    template <typename T, typename K>
    std::unique_ptr<Assign> makeAssignBinExpr(int num, Var LHS, BinOp op, T BLHS, K BRHS) {
        return makeAssign(num, LHS, makeBinExpr(op, BLHS, BRHS));
    };

    auto readPrintLst = [](int num1, std::string v1, int num2, std::string v2) {
        auto read = makeRead(num1, v1);
        auto print = makePrint(num2, v2);
        std::vector<std::unique_ptr<Statement>> vect;
        vect.push_back(std::move(read));
        vect.push_back(std::move(print));
        return makeStatementLst(std::move(vect));
    };

    auto makeWhile = [](int num, std::unique_ptr<CondExpr> cond, StmtLst& blk) {
        return std::make_unique<While>(num, std::move(cond), std::move(blk));
    };

    auto makeProcedure = [](std::string name, StmtLst& blk) {
        return std::make_unique<Procedure>(name, std::move(blk));
    };


    TEST_CASE("Design extractor Test") {
        // Construct a simple AST;
        /**
         * while (v1 > 11) {
         *     read v1;
         *	   print v1;
         * }
         *
         */
        SECTION("whileBlk walking test") {
            auto relExpr = makeRelExpr(RelOp::GT, Var("v1"), Const(11)); // v1 > 11;
            auto stmtlst = readPrintLst(2, "v1", 3, "v3");
            auto whileBlk = makeWhile(1, std::move(relExpr), std::move(stmtlst));
            TreeWalker tw;
            whileBlk->accept(tw);
            // variable extractions
            REQUIRE(tw.vars == std::set<std::string>({ "v1", "v3" }));
        }
        

        // Construct a more complex AST;
        /**
         * procedure main {
         *     read x;
         *     sum = 0;
         *     while (x > 0) {
         *         print x;
         *         remainder = x % 2;
         *         digit = x % 10;
         *         if (remainder == 0) then {
         *             sum = sum + digit
         *         } else {
         *             
         *         }
         *         x = x / 10;
         *     }
         *     print sum;
         * }
         * 
         */
        SECTION("Complex AST test") {
            Logger() << "Walking complex AST Tree";

            std::vector<std::unique_ptr<Statement>> proclst;
            proclst.push_back(makeRead(1, "x"));
            proclst.push_back(makeAssign(2, Var("sum"), makeBasic<Const>(0)));
             
            std::vector<std::unique_ptr<Statement>> whileLst;
            whileLst.push_back(makePrint(4, "x"));
            whileLst.push_back(makeAssignBinExpr(5, Var("remainder"), BinOp::MOD, Var("x"), Const(2)));
            whileLst.push_back(makeAssignBinExpr(6, Var("digit"), BinOp::MOD, Var("x"), Const(10)));

            std::vector<std::unique_ptr<Statement>> thenLst;
            std::vector<std::unique_ptr<Statement>> elseLst;
            thenLst.push_back(makeAssignBinExpr(8, Var("sum"), BinOp::MOD, Var("sum"), Var("digit")));

            whileLst.push_back(std::make_unique<If>(7,
                makeRelExpr(RelOp::EQ, Var("remainder"), Const(0)),
                StmtLst(std::move(thenLst)),
                StmtLst(std::move(elseLst))
                ));
            whileLst.push_back(makeAssignBinExpr(9, Var("x"), BinOp::DIVIDE, Var("x"), Const(10)));

            auto whileCond = makeRelExpr(RelOp::GT, Var("x"), Const(0));
            auto whileStmt = makeWhile(3, std::move(whileCond), StmtLst(std::move(whileLst)));
            proclst.push_back(std::move(whileStmt));

            proclst.push_back(makePrint(10, "sum"));

            auto program = std::make_unique<Program>(makeProcedure("main", StmtLst(std::move(proclst))));


            TreeWalker tw;
            program->accept(tw);

            std::set<std::string> expectedVars = { "x", "remainder", "digit", "sum" };
            REQUIRE(tw.vars == expectedVars);
        }
    }

}