#include <iostream>
#include <vector>
#include <cstdarg>
#include <stdio.h>

#include "catch.hpp"
#include "Parser/AST.h"
#include "DesignExtractor/PKBStrategy.h"
#include "DesignExtractor/DesignExtractor.h"
#include "DesignExtractor/EntityExtractor/VariableExtractor.h"
#include "DesignExtractor/RelationshipExtractor/ModifiesExtractor.h"
#include "DesignExtractor/RelationshipExtractor/UsesExtractor.h"
#include "PKB.h"
#include "logging.h"

#define TEST_LOG Logger() << "TestDesignExtractor.cpp "


class TestPKBStrategy : public PKBStrategy {
public:
    std::set<STMT_LO> statements;
    std::set<std::string> variables;
    std::map<PKBRelationship, std::set<std::pair<Content, Content>>> relationships;

    void insertStatement(STMT_LO stmt) override {
        statements.insert(stmt);
    };
    void insertVariable(std::string name) override {
        variables.insert(name);
    };
    void insertRelationship(PKBRelationship type, Content arg1, Content arg2) override {
        relationships[type].insert(std::make_pair<>(arg1, arg2));
    };
};

TEST_CASE("TestPKBStrategy Test") {
    TestPKBStrategy pkb;
    std::set<STMT_LO> stmts = {
        STMT_LO{1, StatementType::Assignment},
        STMT_LO{2, StatementType::Read},
        STMT_LO{3, StatementType::Print},
        STMT_LO{4, StatementType::If},
        STMT_LO{5, StatementType::While},
    };
    for (auto stmt : stmts) {
        pkb.insertStatement(stmt);
    }
    REQUIRE(pkb.statements == stmts);

    std::set<std::string> vars = {
        "x",
        "x123",
        "jaosidjfaoisdjfiaosdjfioasjd"
    };
    for (auto var : vars) {
        pkb.insertVariable(var);
    }
    REQUIRE(pkb.variables == vars);

    
    auto p = [](auto a1, auto a2) {
        return std::make_pair<>(a1, a2);
    };
    std::set<std::pair<Content, Content>> relationships = {
        p(STMT_LO{1, StatementType::Assignment}, VAR_NAME{"X"}),
        p(STMT_LO{1, StatementType::Assignment}, STMT_LO{2, StatementType::Read}),
        p(PROC_NAME{"main"}, VAR_NAME("x"))
    };
    for (auto relationship : relationships) {
        pkb.insertRelationship(PKBRelationship::MODIFIES, relationship.first, relationship.second);
    }
    REQUIRE(pkb.relationships[PKBRelationship::MODIFIES] == relationships);
}

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

        TestPKBStrategy pkb;
        auto p = [] (auto p1, auto p2) {
            return std::make_pair<>(p1,p2);
        };  // Helper method to make pairs.

        SECTION("whileBlk walking test") {
            TEST_LOG << "Walking simple while AST";
            auto relExpr = make<RelExpr>(RelOp::GT, make<Var>("v1"), make<Const>(11));  // v1 > 11
            auto stmtlst = makeStmts(
                make<Read>(2, make<Var>("v1")), 
                make<Print>(3, make<Var>("v3"))
            );
            auto whileBlk = make<While>(1, std::move(relExpr), std::move(stmtlst));
            auto ve = std::make_shared<VariableExtractor>(&pkb);
            whileBlk->accept(ve);
            // variable extractions
            
            REQUIRE(pkb.variables == std::set<std::string>({"v1", "v3"}));
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
                make<Print>(11, make<Var>("sum"))
            ));

            auto program = std::make_unique<Program>(std::move(procedure));

            SECTION("Variable extractor test") {
                auto ve = std::make_shared<VariableExtractor>(&pkb);
                program->accept(ve);

                std::set<std::string> expectedVars = { "x", "remainder", "digit", "sum" };
                REQUIRE(pkb.variables == expectedVars);
            }

            SECTION("Modifies extractor test") {
                auto me = std::make_shared<ModifiesExtractor>(&pkb);
                program->accept(me);

                std::set<std::pair<Content, Content>> expected = {
                    p(PROC_NAME{"main"}, VAR_NAME{"x"}),
                    p(PROC_NAME{"main"}, VAR_NAME{"sum"}),
                    p(PROC_NAME{"main"}, VAR_NAME{"remainder"}),
                    p(PROC_NAME{"main"}, VAR_NAME{"digit"}),

                    p(STMT_LO{1, StatementType::Read}, VAR_NAME{"x"}),
                    p(STMT_LO{2, StatementType::Assignment}, VAR_NAME{"sum"}),
                    p(STMT_LO{3, StatementType::While}, VAR_NAME{"digit"}),
                    p(STMT_LO{3, StatementType::While}, VAR_NAME{"remainder"}),
                    p(STMT_LO{3, StatementType::While}, VAR_NAME{"sum"}),
                    p(STMT_LO{3, StatementType::While}, VAR_NAME{"x"}),
                    p(STMT_LO{5, StatementType::Assignment}, VAR_NAME{"remainder"}),
                    p(STMT_LO{6, StatementType::Assignment}, VAR_NAME{"digit"}),
                    p(STMT_LO{7, StatementType::If}, VAR_NAME{"sum"}),
                    p(STMT_LO{8, StatementType::Assignment}, VAR_NAME{"sum"}),
                    p(STMT_LO{10, StatementType::Assignment}, VAR_NAME{"x"}),

                };
                REQUIRE(pkb.relationships[PKBRelationship::MODIFIES] == expected);
            }

            SECTION("Uses extractor test") {
                auto ue = std::make_shared<UsesExtractor>(&pkb);
                program->accept(ue);

                std::set<std::pair<Content, Content>> expected = {
                    p(PROC_NAME{"main"}, VAR_NAME{"x"}),
                    p(PROC_NAME{"main"}, VAR_NAME{"sum"}),
                    p(PROC_NAME{"main"}, VAR_NAME{"remainder"}),
                    p(PROC_NAME{"main"}, VAR_NAME{"digit"}),

                    p(STMT_LO{3, StatementType::While}, VAR_NAME{"digit"}),
                    p(STMT_LO{3, StatementType::While}, VAR_NAME{"remainder"}),
                    p(STMT_LO{3, StatementType::While}, VAR_NAME{"sum"}),
                    p(STMT_LO{3, StatementType::While}, VAR_NAME{"x"}),
                    p(STMT_LO{4, StatementType::Print}, VAR_NAME{"x"}),
                    p(STMT_LO{5, StatementType::Assignment}, VAR_NAME{"x"}),
                    p(STMT_LO{6, StatementType::Assignment}, VAR_NAME{"x"}),
                    p(STMT_LO{7, StatementType::If}, VAR_NAME{"digit"}),
                    p(STMT_LO{7, StatementType::If}, VAR_NAME{"remainder"}),
                    p(STMT_LO{7, StatementType::If}, VAR_NAME{"sum"}),
                    p(STMT_LO{7, StatementType::If}, VAR_NAME{"x"}),
                    p(STMT_LO{8, StatementType::Assignment}, VAR_NAME{"sum"}),
                    p(STMT_LO{8, StatementType::Assignment}, VAR_NAME{"digit"}),
                    p(STMT_LO{9, StatementType::Print}, VAR_NAME{"x"}),
                    p(STMT_LO{10, StatementType::Assignment}, VAR_NAME{"x"}),
                    p(STMT_LO{11, StatementType::Print}, VAR_NAME{"sum"}),
                };

                REQUIRE(pkb.relationships[PKBRelationship::USES] == expected);
            }
        }
    }

}  // namespace AST

