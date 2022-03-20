#include <iostream>
#include <vector>
#include <cstdarg>
#include <stdio.h>
#include <set>

#include "catch.hpp"
#include "Parser/AST.h"
#include "DesignExtractor/PKBStrategy.h"
#include "DesignExtractor/DesignExtractor.h"
#include "DesignExtractor/EntityExtractor/EntityExtractor.h"
#include "DesignExtractor/RelationshipExtractor/RelationshipExtractor.h"
#include "DesignExtractor/PatternMatcher.h"
#include "DesignExtractor/CFG/CFG.h"
#include "PKB.h"
#include "logging.h"

#define TEST_LOG Logger() << "TestDesignExtractor.cpp "

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

namespace sp {
namespace de = design_extractor;

using de::VariableExtractorModule;
using de::StatementExtractorModule;
using de::ConstExtractorModule;
using de::ProcedureExtractorModule;
using de::UsesExtractorModule;
using de::ModifiesExtractorModule;
using de::ParentExtractorModule;
using de::FollowsExtractorModule;
using de::NextExtractorModule;

class TestPKBStrategy : public de::PKBStrategy {
public:
    std::set<STMT_LO> statements;
    std::set<std::string> variables;
    std::map<PKBEntityType, std::set<Content>> entities;
    std::map<PKBRelationship, std::set<std::pair<Content, Content>>> relationships;

    void insertEntity(Content entity) override {
        std::visit(overloaded {
            [&](VAR_NAME &item) { entities[PKBEntityType::VARIABLE].emplace(item); },
            [&](STMT_LO &item) { entities[PKBEntityType::STATEMENT].emplace(item); },
            [&](PROC_NAME &item) { entities[PKBEntityType::PROCEDURE].emplace(item); },
            [&](CONST &item) { entities[PKBEntityType::CONST].emplace(item); },
            [](auto &item) {}
        }, entity);
    };

    void insertRelationship(PKBRelationship type, Content arg1, Content arg2) override {
        relationships[type].insert(std::make_pair<>(arg1, arg2));
    };
};

template <typename T>
std::set<T> setDiff(std::set<T> set1, std::set<T> set2) {
    std::set<T> diff;
    std::set_difference(
        set1.begin(), set1.end(),
        set2.begin(), set2.end(),
        std::inserter(diff, diff.end())
    );
    return diff;
}

TEST_CASE("TestPKBStrategy Test") {
    TestPKBStrategy pkbStrategy;
    std::set<Content> stmts = {
        STMT_LO{1, StatementType::Assignment},
        STMT_LO{2, StatementType::Read},
        STMT_LO{3, StatementType::Print},
        STMT_LO{4, StatementType::If},
        STMT_LO{5, StatementType::While},
    };
    for (auto stmt : stmts) {
        pkbStrategy.insertEntity(stmt);
    }
    REQUIRE(pkbStrategy.entities[PKBEntityType::STATEMENT] == stmts);

    std::set<Content> vars = {
        VAR_NAME{"x"},
        VAR_NAME{"x123"},
        VAR_NAME{"jaosidjfaoisdjfiaosdjfioasjd"}
    };
    for (auto var : vars) {
        pkbStrategy.insertEntity(var);
    }
    REQUIRE(pkbStrategy.entities[PKBEntityType::VARIABLE] == vars);

    
    auto p = [](auto a1, auto a2) {
        return std::make_pair<>(a1, a2);
    };
    std::set<std::pair<Content, Content>> relationships = {
        p(STMT_LO{1, StatementType::Assignment}, VAR_NAME{"X"}),
        p(STMT_LO{1, StatementType::Assignment}, STMT_LO{2, StatementType::Read}),
        p(PROC_NAME{"main"}, VAR_NAME("x"))
    };
    for (auto relationship : relationships) {
        pkbStrategy.insertRelationship(PKBRelationship::MODIFIES, relationship.first, relationship.second);
    }
    REQUIRE(pkbStrategy.relationships[PKBRelationship::MODIFIES] == relationships);
}

namespace ast {
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

        TestPKBStrategy pkbStrategy;
        auto p = [] (auto p1, auto p2) {
            return std::make_pair<>(p1, p2);
        };  // Helper method to make pairs.

        SECTION("whileBlk walking test") {
            TEST_LOG << "Walking simple while AST";
            auto relExpr = make<RelExpr>(RelOp::GT, make<Var>("v1"), make<Const>(11));  // v1 > 11
            auto stmtlst = makeStmts(
                make<Read>(2, make<Var>("v1")), 
                make<Print>(3, make<Var>("v3"))
            );
            auto whileBlk = make<While>(1, std::move(relExpr), std::move(stmtlst));
            auto ve = std::make_unique<VariableExtractorModule>(&pkbStrategy);
            ve->extract(whileBlk.get());
            // variable extractions
            
            REQUIRE(
                pkbStrategy.entities[PKBEntityType::VARIABLE] == std::set<Content>(
                    {VAR_NAME{"v1"}, VAR_NAME{"v3"}}
                )
            );
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

            auto program = makeProgram(std::move(procedure));

            SECTION("Variable extractor test") {
                auto ve = std::make_unique<VariableExtractorModule>(&pkbStrategy);
                ve->extract(program.get());

                std::set<Content> expectedVars = {
                    VAR_NAME{"x"},
                    VAR_NAME{"remainder"},
                    VAR_NAME{"digit"},
                    VAR_NAME{"sum"} 
                };
                
                REQUIRE(pkbStrategy.entities[PKBEntityType::VARIABLE] == expectedVars);
            }

            SECTION("Const extractor test") {
                auto ce = std::make_unique<ConstExtractorModule>(&pkbStrategy);
                ce->extract(program.get());

                std::set<Content> expectedConsts = { CONST{0}, CONST{2}, CONST{10} };
                REQUIRE(pkbStrategy.entities[PKBEntityType::CONST] == expectedConsts);
            }

            SECTION("Procedure extractor test") {
                auto pe = std::make_unique<ProcedureExtractorModule>(&pkbStrategy);
                pe->extract(program.get());
                
                std::set<Content> expectedProcs = { PROC_NAME{"main"} };
                REQUIRE(pkbStrategy.entities[PKBEntityType::PROCEDURE] == expectedProcs);
            }

            SECTION("Statement extractor test") {
                auto se = std::make_unique<StatementExtractorModule>(&pkbStrategy);
                se->extract(program.get());

                std::set<Content> expectedStatements = {
                    STMT_LO{1, StatementType::Read, "x"},
                    STMT_LO{2, StatementType::Assignment},
                    STMT_LO{3, StatementType::While},
                    STMT_LO{4, StatementType::Print, "x"},
                    STMT_LO{5, StatementType::Assignment},
                    STMT_LO{6, StatementType::Assignment},
                    STMT_LO{7, StatementType::If},
                    STMT_LO{8, StatementType::Assignment},
                    STMT_LO{9, StatementType::Print, "x"},
                    STMT_LO{10, StatementType::Assignment},
                    STMT_LO{11, StatementType::Print, "sum"},
                };
            }

            SECTION("Modifies extractor test") {
                auto me = std::make_unique<ModifiesExtractorModule>(&pkbStrategy);
                me->extract(program.get());

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
                REQUIRE(pkbStrategy.relationships[PKBRelationship::MODIFIES] == expected);
            }

            SECTION("Uses extractor test") {
                auto ue = std::make_unique<UsesExtractorModule>(&pkbStrategy);
                ue->extract(program.get());

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

                REQUIRE(pkbStrategy.relationships[PKBRelationship::USES] == expected);
            }
        
            SECTION("Follows extractor test") {
                auto fe = std::make_unique<FollowsExtractorModule>(&pkbStrategy);
                fe->extract(program.get());

                std::set<std::pair<Content, Content>> expected = {
                    p(STMT_LO{1, StatementType::Read}, STMT_LO{2, StatementType::Assignment}),
                    p(STMT_LO{2, StatementType::Assignment}, STMT_LO{3, StatementType::While}),
                    p(STMT_LO{3, StatementType::While}, STMT_LO{11, StatementType::Print}),
                    p(STMT_LO{4, StatementType::Print}, STMT_LO{5, StatementType::Assignment}),
                    p(STMT_LO{5, StatementType::Assignment}, STMT_LO{6, StatementType::Assignment}),
                    p(STMT_LO{6, StatementType::Assignment}, STMT_LO{7, StatementType::If}),
                    p(STMT_LO{7, StatementType::If}, STMT_LO{10, StatementType::Assignment}),
                };
                REQUIRE(pkbStrategy.relationships[PKBRelationship::FOLLOWS] == expected);
            }

            SECTION("Parents extractor test") {
                auto pe = std::make_unique<ParentExtractorModule>(&pkbStrategy);
                pe->extract(program.get());

                std::set<std::pair<Content, Content>> expected = {
                    p(STMT_LO{3, StatementType::While}, STMT_LO{4, StatementType::Print}),
                    p(STMT_LO{3, StatementType::While}, STMT_LO{5, StatementType::Assignment}),
                    p(STMT_LO{3, StatementType::While}, STMT_LO{6, StatementType::Assignment}),
                    p(STMT_LO{3, StatementType::While}, STMT_LO{7, StatementType::If}),
                    p(STMT_LO{7, StatementType::If}, STMT_LO{8, StatementType::Assignment}),
                    p(STMT_LO{7, StatementType::If}, STMT_LO{9, StatementType::Print}),
                    p(STMT_LO{3, StatementType::While}, STMT_LO{10, StatementType::Assignment}),
                };
                REQUIRE(pkbStrategy.relationships[PKBRelationship::PARENT] == expected);
            }

            SECTION("Next extractor test") {
                auto ne = std::make_unique<NextExtractorModule>(&pkbStrategy);
                auto cfg = std::make_unique<cfg::CFGExtractor>()->extract(program.get());
                ne->extract(&cfg);
                std::set<std::pair<Content, Content>> expected = {
                    p(STMT_LO{1, StatementType::Read}, STMT_LO{2, StatementType::Assignment}),
                    p(STMT_LO{2, StatementType::Assignment}, STMT_LO{3, StatementType::While}),
                    p(STMT_LO{3, StatementType::While}, STMT_LO{4, StatementType::Print}),
                    p(STMT_LO{4, StatementType::Print}, STMT_LO{5, StatementType::Assignment}),
                    p(STMT_LO{5, StatementType::Assignment}, STMT_LO{6, StatementType::Assignment}),
                    p(STMT_LO{6, StatementType::Assignment}, STMT_LO{7, StatementType::If}),
                    p(STMT_LO{7, StatementType::If}, STMT_LO{8, StatementType::Assignment}),
                    p(STMT_LO{7, StatementType::If}, STMT_LO{9, StatementType::Print}),
                    p(STMT_LO{8, StatementType::Assignment}, STMT_LO{10, StatementType::Assignment}),
                    p(STMT_LO{9, StatementType::Print}, STMT_LO{10, StatementType::Assignment}),
                    p(STMT_LO{10, StatementType::Assignment}, STMT_LO{3, StatementType::While}),
                    p(STMT_LO{3, StatementType::While}, STMT_LO{11, StatementType::Print}),
                };
                REQUIRE(pkbStrategy.relationships[PKBRelationship::NEXT] == expected);
            }
        }    
        
        /**
         * procedure main {
         *    call foo;
         *    call bar;
         * }
         * procedure foo {
         *    call gee;
         * }
         * procedure bar {
         *    call gee;
         *    call foo;
         * }
         * procedure gee {
         *    read x;
         *    print y;
         * }
         * 
         */
        SECTION("Multi procedure test 1") {
            auto program = makeProgram(
                make<ast::Procedure>(
                    "main", 
                    ast::makeStmts(
                        make<ast::Call>(1, "foo"),
                        make<ast::Call>(2, "bar")
                    )
                ),
                make<ast::Procedure>(
                    "foo", 
                    ast::makeStmts(
                        make<ast::Call>(3, "gee")
                    )
                ),
                make<ast::Procedure>(
                    "bar", 
                    ast::makeStmts(
                        make<ast::Call>(4, "gee"),
                        make<ast::Call>(5, "foo")
                    )
                ),
                make<ast::Procedure>(
                    "gee", 
                    ast::makeStmts(
                        make<ast::Read>(6, make<ast::Var>("x")),
                        make<ast::Print>(7, make<ast::Var>("y"))
                    )
                )
            );
            std::set<std::pair<Content, Content>> expected;

            ModifiesExtractorModule me(&pkbStrategy);
            me.extract(program.get());

            expected = {
                p(STMT_LO{1, StatementType::Call}, VAR_NAME{"x"}),
                p(STMT_LO{2, StatementType::Call}, VAR_NAME{"x"}),
                p(STMT_LO{3, StatementType::Call}, VAR_NAME{"x"}),
                p(STMT_LO{4, StatementType::Call}, VAR_NAME{"x"}),
                p(STMT_LO{5, StatementType::Call}, VAR_NAME{"x"}),
                p(STMT_LO{6, StatementType::Read}, VAR_NAME{"x"}),
                p(PROC_NAME{"bar"}, VAR_NAME{"x"}),
                p(PROC_NAME{"foo"}, VAR_NAME{"x"}),
                p(PROC_NAME{"gee"}, VAR_NAME{"x"}),
                p(PROC_NAME{"main"}, VAR_NAME{"x"})
            };
            REQUIRE(pkbStrategy.relationships[PKBRelationship::MODIFIES] == expected);
            
            UsesExtractorModule ue(&pkbStrategy);
            ue.extract(program.get());

            expected = {
                p(STMT_LO{1, StatementType::Call}, VAR_NAME{"y"}),
                p(STMT_LO{2, StatementType::Call}, VAR_NAME{"y"}),
                p(STMT_LO{3, StatementType::Call}, VAR_NAME{"y"}),
                p(STMT_LO{4, StatementType::Call}, VAR_NAME{"y"}),
                p(STMT_LO{5, StatementType::Call}, VAR_NAME{"y"}),
                p(STMT_LO{7, StatementType::Print}, VAR_NAME{"y"}),
                p(PROC_NAME{"bar"}, VAR_NAME{"y"}),
                p(PROC_NAME{"foo"}, VAR_NAME{"y"}),
                p(PROC_NAME{"gee"}, VAR_NAME{"y"}),
                p(PROC_NAME{"main"}, VAR_NAME{"y"})
            };
            REQUIRE(pkbStrategy.relationships[PKBRelationship::USES] == expected);

            // Regression test for #287.
            FollowsExtractorModule fe(&pkbStrategy);
            fe.extract(program.get());
            expected = {
                p(STMT_LO{1, StatementType::Call}, STMT_LO{2, StatementType::Call}),
                p(STMT_LO{4, StatementType::Call}, STMT_LO{5, StatementType::Call}),
                p(STMT_LO{6, StatementType::Read}, STMT_LO{7, StatementType::Print}),
            };

            auto diff = setDiff(pkbStrategy.relationships[PKBRelationship::FOLLOWS], expected);
            REQUIRE(pkbStrategy.relationships[PKBRelationship::FOLLOWS] == expected);


            de::CallsExtractorModule ce(&pkbStrategy);
            ce.extract(program.get());
            expected = {
                p(PROC_NAME{"main"}, PROC_NAME{"foo"}),
                p(PROC_NAME{"main"}, PROC_NAME{"bar"}),
                p(PROC_NAME{"foo"}, PROC_NAME{"gee"}),
                p(PROC_NAME{"bar"}, PROC_NAME{"gee"}),
                p(PROC_NAME{"bar"}, PROC_NAME{"foo"})
            };

            REQUIRE(pkbStrategy.relationships[PKBRelationship::CALLS] == expected);

            NextExtractorModule ne(&pkbStrategy);
            auto cfgs = cfg::CFGExtractor().extract(program.get());
            ne.extract(&cfgs);
            expected = {
                p(STMT_LO{1, StatementType::Call}, STMT_LO{2, StatementType::Call}),
                p(STMT_LO{4, StatementType::Call}, STMT_LO{5, StatementType::Call}),
                p(STMT_LO{6, StatementType::Read}, STMT_LO{7, StatementType::Print}),
            };

            REQUIRE(pkbStrategy.relationships[PKBRelationship::NEXT] == expected);
        }

        /**
         * procedure main {
         *      while (m1 > 0) {
         *          call foo;
         *          m2 = m3;
         *      }
         * }
         * procedure foo {
         *      f1 = f2;
         * }
         */
        SECTION("Multi procedure test 2") {
            auto program = makeProgram(
                make<Procedure>("main", makeStmts(
                    make<While>(1,
                        make<RelExpr>(RelOp::GT, make<Var>("m1"), make<Const>(0)),
                        makeStmts(
                            make<Call>(2, "foo"),
                            make<Assign>(3, make<Var>("m2"), make<Var>("m3"))
                        )
                    )
                )),
                make<Procedure>("foo", makeStmts(
                    make<Assign>(4, make<Var>("f1"), make<Var>("f2"))
                ))
            );

            std::set<Content> expectedProcs = {
                PROC_NAME{"main"},
                PROC_NAME{"foo"}
            };
            ProcedureExtractorModule(&pkbStrategy).extract(program.get());
            REQUIRE(pkbStrategy.entities[PKBEntityType::PROCEDURE] == expectedProcs);

            std::set<Content> expectedStatements = {
                STMT_LO{1, StatementType::While},
                STMT_LO{2, StatementType::Call, "foo"},
                STMT_LO{3, StatementType::Assignment},
                STMT_LO{4, StatementType::Assignment},
            };
            StatementExtractorModule(&pkbStrategy).extract(program.get());
            auto extractedStatements = pkbStrategy.entities[PKBEntityType::STATEMENT];
            REQUIRE(extractedStatements == expectedStatements);
            // If call statement without attribute, it's wrong.
            REQUIRE(extractedStatements.find(STMT_LO{2, StatementType::Call}) == extractedStatements.end());
            REQUIRE(extractedStatements.find(STMT_LO{2, StatementType::Call, "foo"}) != extractedStatements.end());

            std::set<std::pair<Content, Content>> expected;
            ModifiesExtractorModule me(&pkbStrategy);
            me.extract(program.get());

            expected = {
                p(STMT_LO{1, StatementType::While}, VAR_NAME{"f1"}),
                p(STMT_LO{1, StatementType::While}, VAR_NAME{"m2"}),
                p(STMT_LO{2, StatementType::Call}, VAR_NAME{"f1"}),
                p(STMT_LO{3, StatementType::Assignment}, VAR_NAME{"m2"}),
                p(STMT_LO{4, StatementType::Assignment}, VAR_NAME{"f1"}),
                p(PROC_NAME{"foo"}, VAR_NAME{"f1"}),
                p(PROC_NAME{"main"}, VAR_NAME{"f1"}),
                p(PROC_NAME{"main"}, VAR_NAME{"m2"})
            };
            REQUIRE(pkbStrategy.relationships[PKBRelationship::MODIFIES] == expected);

            UsesExtractorModule ue(&pkbStrategy);
            ue.extract(program.get());
            expected = {
                p(STMT_LO{1, StatementType::While}, VAR_NAME{"m1"}),
                p(STMT_LO{1, StatementType::While}, VAR_NAME{"m3"}),
                p(STMT_LO{1, StatementType::While}, VAR_NAME{"f2"}),
                p(STMT_LO{2, StatementType::Call}, VAR_NAME{"f2"}),
                p(STMT_LO{3, StatementType::Assignment}, VAR_NAME{"m3"}),
                p(STMT_LO{4, StatementType::Assignment}, VAR_NAME{"f2"}),
                p(PROC_NAME{"foo"}, VAR_NAME{"f2"}),
                p(PROC_NAME{"main"}, VAR_NAME{"f2"}),
                p(PROC_NAME{"main"}, VAR_NAME{"m1"}),
                p(PROC_NAME{"main"}, VAR_NAME{"m3"})
            };
            REQUIRE(pkbStrategy.relationships[PKBRelationship::USES] == expected);

            NextExtractorModule ne(&pkbStrategy);
            auto cfgs = cfg::CFGExtractor().extract(program.get());
            ne.extract(&cfgs);
            expected = {
                p(STMT_LO{1, StatementType::While}, STMT_LO{2, StatementType::Call}),
                p(STMT_LO{2, StatementType::Call}, STMT_LO{3, StatementType::Assignment}),
                p(STMT_LO{3, StatementType::Assignment}, STMT_LO{1, StatementType::While}),
            };

            REQUIRE(pkbStrategy.relationships[PKBRelationship::NEXT] == expected);
        }
    
        /**
         * procedure a {
         *      read x;
         * }
         * procedure b {
         *      call a;
         * }
         * procedure c {
         *      read y;
         * }
         * procedure d {
         *      call c;
         * }
         */
        SECTION("Regression test for #292") {
            auto program = makeProgram(
                make<ast::Procedure>(
                    "a", 
                    ast::makeStmts(
                        make<ast::Read>(1, make<ast::Var>("x"))
                    )
                ),
                make<ast::Procedure>(
                    "b", 
                    ast::makeStmts(
                        make<ast::Call>(2, "a")
                    )
                ),
                make<ast::Procedure>(
                    "c", 
                    ast::makeStmts(
                        make<ast::Read>(3,  make<ast::Var>("y"))
                    )
                ),
                make<ast::Procedure>(
                    "d", 
                    ast::makeStmts(
                        make<ast::Call>(4, "c")
                    )
                )
            );
            std::set<std::pair<Content, Content>> expected;

            ModifiesExtractorModule me(&pkbStrategy);
            me.extract(program.get());
            expected = {
                p(PROC_NAME{"a"}, VAR_NAME{"x"}),
                p(PROC_NAME{"b"}, VAR_NAME{"x"}),
                p(PROC_NAME{"c"}, VAR_NAME{"y"}),
                p(PROC_NAME{"d"}, VAR_NAME{"y"}),
                p(PROC_NAME{"d"}, VAR_NAME{"y"}),
                p(STMT_LO{1, StatementType::Read}, VAR_NAME{"x"}),
                p(STMT_LO{2, StatementType::Call}, VAR_NAME{"x"}),
                p(STMT_LO{3, StatementType::Read}, VAR_NAME{"y"}),
                p(STMT_LO{4, StatementType::Call}, VAR_NAME{"y"})
            };
            REQUIRE(pkbStrategy.relationships[PKBRelationship::MODIFIES] == expected);
        }

        /**
         * procedure a {
         *      if (x > 1)  then {
         *          read x;
         *      } else {
         *          if (x > 1) then {
         *              read x;
         *          } else {
         *              read x;
         *          }
         *      }
         * }
         */
        SECTION("Regression test for #303 and #306") {
            auto program = makeProgram(
                make<ast::Procedure>(
                    "a", 
                    ast::makeStmts(
                        make<ast::If>(1, make<ast::RelExpr>(RelOp::GT, make<ast::Var>("x"), make<ast::Const>(1)),
                            makeStmts(
                                make<ast::Read>(2, make<ast::Var>("x"))
                            ),
                            makeStmts(
                                make<ast::If>(
                                    3, 
                                    make<ast::RelExpr>(
                                        RelOp::GT,
                                        make<ast::Var>("x"),
                                        make<ast::Const>(1)
                                    ),
                                    makeStmts(
                                        make<ast::Read>(4, make<ast::Var>("x"))
                                    ),
                                    makeStmts(
                                        make<ast::Read>(5, make<ast::Var>("x"))
                                    )
                                )
                            )
                        )
                    )
                )
            );
            std::set<std::pair<Content, Content>> expected;

            NextExtractorModule ne(&pkbStrategy);
            auto cfgs = cfg::CFGExtractor().extract(program.get());
            ne.extract(&cfgs);
            
            expected = {
                p(STMT_LO{1, StatementType::If}, STMT_LO{2, StatementType::Read}),
                p(STMT_LO{1, StatementType::If}, STMT_LO{3, StatementType::If}),
                p(STMT_LO{3, StatementType::If}, STMT_LO{4, StatementType::Read}),
                p(STMT_LO{3, StatementType::If}, STMT_LO{5, StatementType::Read}),
            };
            REQUIRE(pkbStrategy.relationships[PKBRelationship::NEXT] == expected);
        }

        /**
         * procedure a {
         *     read x;
         * }
         * procedure b {
         *     call a;
         * }
         * procedure h {
         *     call b;
         * }
         * procedure g {
         *     call a;
         * }
         * 
         * 
         * procedure c {
         *     read y;
         * }
         * procedure d {
         *     call c;
         * }
         * 
         * 
         * procedure e {
         *     read z;
         * }
         * procedure f {
         *     call e;
         * }
         * procedure i {
         *     call f
         * }
         * 
         * 
         * procedure j {
         *     read w;
         * }
         */
        SECTION("Test >= 3 disjoint call graphs") {
            auto program = makeProgram(
                // first disjoint graph
                make<ast::Procedure>(
                    "a", 
                    ast::makeStmts(
                        make<ast::Read>(1, make<ast::Var>("x"))
                    )
                ),
                make<ast::Procedure>(
                    "b", 
                    ast::makeStmts(
                        make<ast::Call>(2, "a")
                    )
                ),
                make<ast::Procedure>(
                    "h", 
                    ast::makeStmts(
                        make<ast::Call>(3, "b")
                    )
                ),
                make<ast::Procedure>(
                    "g", 
                    ast::makeStmts(
                        make<ast::Call>(4, "a")
                    )
                ),
                // second disjoint graph
                make<ast::Procedure>(
                    "c", 
                    ast::makeStmts(
                        make<ast::Read>(5, make<ast::Var>("y"))
                    )
                ),
                make<ast::Procedure>(
                    "d", 
                    ast::makeStmts(
                        make<ast::Call>(6, "c")
                    )
                ),
                // third disjoint graph
                make<ast::Procedure>(
                    "e", 
                    ast::makeStmts(
                        make<ast::Read>(7, make<ast::Var>("z"))
                    )
                ),
                make<ast::Procedure>(
                    "f", 
                    ast::makeStmts(
                        make<ast::Call>(8, "e")
                    )
                ),
                make<ast::Procedure>(
                    "i", 
                    ast::makeStmts(
                        make<ast::Call>(9, "f")
                    )
                ),
                make<ast::Procedure>(
                    "j", 
                    ast::makeStmts(
                        make<ast::Read>(10, make<ast::Var>("w"))
                    )
                )
            );
            std::set<std::pair<Content, Content>> expected;

            ModifiesExtractorModule me(&pkbStrategy);
            me.extract(program.get());
            expected = {
                p(PROC_NAME{"a"}, VAR_NAME{"x"}),
                p(PROC_NAME{"b"}, VAR_NAME{"x"}),
                p(PROC_NAME{"g"}, VAR_NAME{"x"}),
                p(PROC_NAME{"h"}, VAR_NAME{"x"}),
                p(PROC_NAME{"c"}, VAR_NAME{"y"}),
                p(PROC_NAME{"d"}, VAR_NAME{"y"}),
                p(PROC_NAME{"e"}, VAR_NAME{"z"}),
                p(PROC_NAME{"f"}, VAR_NAME{"z"}),
                p(PROC_NAME{"i"}, VAR_NAME{"z"}),
                p(PROC_NAME{"j"}, VAR_NAME{"w"}),
                p(STMT_LO{1, StatementType::Read}, VAR_NAME{"x"}),
                p(STMT_LO{2, StatementType::Call}, VAR_NAME{"x"}),
                p(STMT_LO{3, StatementType::Call}, VAR_NAME{"x"}),
                p(STMT_LO{4, StatementType::Call}, VAR_NAME{"x"}),
                p(STMT_LO{5, StatementType::Read}, VAR_NAME{"y"}),
                p(STMT_LO{6, StatementType::Call}, VAR_NAME{"y"}),
                p(STMT_LO{7, StatementType::Read}, VAR_NAME{"z"}),
                p(STMT_LO{8, StatementType::Call}, VAR_NAME{"z"}),
                p(STMT_LO{9, StatementType::Call}, VAR_NAME{"z"}),
                p(STMT_LO{10, StatementType::Read}, VAR_NAME{"w"})
            };
            REQUIRE(pkbStrategy.relationships[PKBRelationship::MODIFIES] == expected);
        }
    }

    TEST_CASE("Pattern matcher test") {
        TEST_LOG << "Testing Assign Pattern matcher";

        // only support _ or string, treat synonyms as _.
        auto extractAssignHelper = [](sp::ast::ASTNode *ast, std::string s1, std::string s2, bool isStrict = false){
            auto field1 = s1 == "_" ? std::nullopt : std::make_optional<>(s1);
            auto field2 = s2 == "_" ? std::nullopt : std::make_optional<>(s2);
            return de::extractAssign(ast, field1, field2, isStrict);
        };

        // assignment: z = 1 + x - 2 + y
        SECTION("Simple Assignment") {
            auto ast = make<Assign>(
                1,
                make<Var>("z"),
                make<BinExpr>(
                    BinOp::PLUS, make<BinExpr>(
                        BinOp::MINUS, make<BinExpr>(
                            BinOp::PLUS,
                            make<Const>(1),
                            make<Var>("x")
                        ),
                        make<Const>(2)
                    ), make<Var>("y")
                )
            );

            REQUIRE(extractAssignHelper(ast.get(), "z", "_").size() == 1);
            REQUIRE(extractAssignHelper(ast.get(), "_", "y").size() == 1);
            REQUIRE(extractAssignHelper(ast.get(), "_", "x-1").size() == 0);
            REQUIRE(extractAssignHelper(ast.get(), "_", "1 +   x - 2 + y").size() == 1);
            REQUIRE(extractAssignHelper(ast.get(), "_", "(1 + x) - 2 + y").size() == 1);
            REQUIRE(extractAssignHelper(ast.get(), "_", "(1 + x) - 2 + y + 1").size() == 0);

            
            REQUIRE(extractAssignHelper(ast.get(), "_", "1 + x - 2 + y", true).size() == 1);
            // bracket should not affect equality if it doesn't change order of opeartion
            REQUIRE(extractAssignHelper(ast.get(), "_", "(1 + x) - 2 + y", true).size() == 1);
            // this changes order of operation, thus no longer equal.
            REQUIRE(extractAssignHelper(ast.get(), "_", "1 + (x - 2) + y", true).size() == 0);

            // strict partial match fails
            REQUIRE(extractAssignHelper(ast.get(), "_", "1 + x - 2", true).size() == 0);
            // non strict partial match pass
            REQUIRE(extractAssignHelper(ast.get(), "_", "1 + x - 2", false).size() == 1);
        }

        // assignment: x = v + x * (y + z) * t
        SECTION("Complex Assignment") {
            auto ast = make<Assign>(
                1,
                make<Var>("x"),
                make<BinExpr>(
                    BinOp::PLUS, 
                    make<Var>("v"),
                    make<BinExpr>(
                        BinOp::MULT,
                        make<BinExpr>(
                            BinOp::MULT,
                            make<Var>("x"),
                            make<BinExpr>(
                                BinOp::PLUS,
                                make<Var>("y"),
                                make<Var>("z")
                            )
                        ),
                        make<Var>("t")
                    )
                )
            );

            REQUIRE(extractAssignHelper(ast.get(), "x", "_").size() == 1);
            REQUIRE(extractAssignHelper(ast.get(), "x", "v").size() == 1);
            REQUIRE(extractAssignHelper(ast.get(), "_", "v").size() == 1);
            REQUIRE(extractAssignHelper(ast.get(), "_", "v+x").size() == 0);
            REQUIRE(extractAssignHelper(ast.get(), "_", "v + x * (y + z) * t").size() == 1);
            REQUIRE(extractAssignHelper(ast.get(), "_", "y + z").size() == 1);
            REQUIRE(extractAssignHelper(ast.get(), "_", "(y + z) * t").size() == 0);
            REQUIRE(extractAssignHelper(ast.get(), "_", "x * (y + z)").size() == 1);

            REQUIRE(extractAssignHelper(ast.get(), "_", "v + x * (y + z) * t", true).size() == 1);
        }
    }

}  // namespace ast
}  // namespace sp
