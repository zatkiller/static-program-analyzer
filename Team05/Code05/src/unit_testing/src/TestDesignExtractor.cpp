#include <iostream>
#include <vector>
#include <cstdarg>
#include <stdio.h>

#include "catch.hpp"
#include "Parser/AST.h"
#include "DesignExtractor/DesignExtractor.h"
#include "DesignExtractor/VariableExtractor.h"
#include "DesignExtractor/ModifiesExtractor.h"
#include "PKB.h"
#include "logging.h"

#define TEST_LOG Logger() << "TestDesignExtractor.cpp "

namespace AST {
    TEST_CASE("Design extractor Test") {
        TEST_LOG << "Testing Design Extractor";
        // Construct a simple AST;
        /**
         * while (v1 > 11) {
         *     read v1;
         *	   print v3;
         * }
         *
         */

        auto pkb = std::make_shared<PKBAdaptor>(std::make_shared<PKB>());

        SECTION("whileBlk walking test") {
            TEST_LOG << "Walking simple while AST";
            auto relExpr = make<RelExpr>(RelOp::GT, make<Var>("v1"), make<Const>(11));  // v1 > 11
            auto stmtlst = makeStmts(
                make<Read>(2, make<Var>("v1")), 
                make<Print>(3, make<Var>("v3"))
            );
            auto whileBlk = make<While>(1, std::move(relExpr), std::move(stmtlst));
            auto ve = std::make_shared<VariableExtractor>(pkb);
            whileBlk->accept(ve);
            // variable extractions
            REQUIRE(ve->getVars() == std::set<std::string>({"v1", "v3"}));
        }


        // Construct a more complex AST;
        /**
         *      procedure main {
         *    1      read x;
         *    2      sum = 0;
         *    3      while (x > 0) {
         *    4          print x;
         *    5          remainder = x % 2;
         *    6          digit = x % 10;
         *    7          if (remainder == 0) then {
         *    8              sum = sum + digit
         *               } else {
         *    9               print x;
         *               }
         *   10          x = x / 10;
         *           }
         *   11      print sum;
         * }
         * 
         */
        SECTION("Complex AST test") {
            TEST_LOG << "Walking complex AST Tree";

            auto procedure = make<Procedure>("main", makeStmts(
                make<Read>(1, make<Var>("x")),
                make<Assign>(2, make<Var>("sum"), make<Const>(0)),
                make<While>(3, make<RelExpr>(RelOp::GT, make<Var>("x"), make<Const>(0)), makeStmts(
                    make<Print>(4, make<Var>("x")),
                    make<Assign>(5, make<Var>("remainder"), make<BinExpr>(BinOp::MOD, make<Var>("x"), make<Const>(2))),
                    make<Assign>(6, make<Var>("digit"), make<BinExpr>(BinOp::MOD, make<Var>("x"), make<Const>(10))),
                    make<If>(7, make<RelExpr>(RelOp::EQ, make<Var>("remainder"), make<Const>(0)), makeStmts(
                        make<Assign>(8, make<Var>("sum"), 
                            make<BinExpr>(BinOp::PLUS, make<Var>("sum"), make<Var>("digit")))
                    ), makeStmts(
                        make<Print>(9, make<Var>("x"))
                    )),
                    make<Assign>(10, make<Var>("x"), make<BinExpr>(BinOp::DIVIDE, make<Var>("x"), make<Const>(10)))
                )),
                make<Print>(11, make<Var>("x"))
            ));

            auto program = std::make_unique<Program>(std::move(procedure));

            SECTION("Variable extractor test") {
                auto ve = std::make_shared<VariableExtractor>(pkb);
                program->accept(ve);

                std::set<std::string> expectedVars = { "x", "remainder", "digit", "sum" };
                REQUIRE(ve->getVars() == expectedVars);
            }

            SECTION("Modifies extractor test") {
                auto me = std::make_shared<ModifiesExtractor>(pkb);
                program->accept(me);

                muTable m;

                m.insert(std::make_pair<>("main", "x"));
                m.insert(std::make_pair<>("main", "sum"));
                m.insert(std::make_pair<>("main", "remainder"));
                m.insert(std::make_pair<>("main", "digit"));
                m.insert(std::make_pair<>(1, "x"));
                m.insert(std::make_pair<>(2, "sum"));
                m.insert(std::make_pair<>(3, "digit"));
                m.insert(std::make_pair<>(3, "remainder"));
                m.insert(std::make_pair<>(3, "sum"));
                m.insert(std::make_pair<>(3, "x"));
                m.insert(std::make_pair<>(5, "remainder"));
                m.insert(std::make_pair<>(6, "digit"));
                m.insert(std::make_pair<>(7, "sum"));
                m.insert(std::make_pair<>(8, "sum"));
                m.insert(std::make_pair<>(10, "x"));

                REQUIRE(me->getModifies() == m);
            }
        }
    }

}  // namespace AST

