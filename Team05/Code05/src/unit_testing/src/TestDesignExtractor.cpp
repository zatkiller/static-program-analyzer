#include <iostream>
#include <vector>
#include <cstdarg>
#include <stdio.h>

#include "catch.hpp"
#include "Parser/AST.h"
#include "DesignExtractor.h"


namespace AST {
    using std::move;

    std::unique_ptr<Var> makeVar(std::string name) {
        return std::make_unique<Var>(name);
    }

    std::unique_ptr<Read> makeRead(int num, std::unique_ptr<Var>& v) {
        return std::make_unique<Read>(num, std::move(v));
    }

    std::unique_ptr<Print> makePrint(int num, std::unique_ptr<Var>& v) {
        return std::make_unique<Print>(num, std::move(v));
    }

    StmtLst makeStatementLst(std::unique_ptr<Statement> s1, std::unique_ptr<Statement> s2) {
        std::vector<std::unique_ptr<Statement>> vect;

        vect.push_back(std::move(s1));
        vect.push_back(std::move(s2));
        return StmtLst( std::move(vect));
    }

    template <typename T, typename K>
    std::unique_ptr<RelExpr> makeRelExpr(RelOp op, T LHS, K RHS) {
        return std::make_unique<RelExpr>(op, std::make_unique<T>(LHS), std::make_unique<K>(RHS));
    }

    auto readPrintLst = [](int num1, std::string v1, int num2, std::string v2) {
        return makeStatementLst(std::move(makeRead(num1, makeVar(v1))), std::move(makeRead(num2, makeVar(v2))));
    };

    auto makeWhile = [](int num, std::unique_ptr<CondExpr> cond, StmtLst& blk) {
        return std::make_unique<While>(num, std::move(cond), std::move(blk));
    };

    TEST_CASE("Design extractor Test") {
        SECTION("IO statement visitor test") {
            std::unique_ptr<IO> read1 = makeRead(1, makeVar("v1"));
            std::unique_ptr<IO> print1 = makePrint(1, makeVar("v1"));

            VariableExtractor ve;
            print1->accept(ve);
            read1->accept(ve);
        }

        // Construct a simple AST;
        /**
         * while (v1 > 11) {
         *     read v1;
         *	   print v1;
         * }
         *
         */
        auto relExpr = makeRelExpr(RelOp::GT, Var("v1"), Const(11)); // v1 > 11;
        auto stmtlst = readPrintLst(2, "v1", 3, "v1");
        auto whileBlk = makeWhile(1, std::move(relExpr), std::move(stmtlst));

        
    }

}