#include <vector>
#include <memory>
#include <unordered_set>

#include "catch.hpp"
#include "DesignExtractor/CFG/CFG.h"
#include "Parser/AST.h"
#include "Parser/Lexer.h"
#include "Parser/Parser.h"

#define TEST_LOG Logger() << "TestDesignExtractor.cpp "

namespace sp {
namespace cfg {
TEST_CASE("CFG Test") {
    // Construction test
    auto node1 = std::make_shared<CFGNode>(1, StatementType::While);
    auto node2 = std::make_shared<CFGNode>(2, StatementType::While);
    auto node3 = std::make_shared<CFGNode>(3, StatementType::Assignment);
    auto node4 = std::make_shared<CFGNode>(4, StatementType::Print);
    auto node5 = std::make_shared<CFGNode>(5, StatementType::Read);
    auto node6 = std::make_shared<CFGNode>(6, StatementType::Call);
    auto node7 = std::make_shared<CFGNode>(7, StatementType::If);
    auto node8 = std::make_shared<CFGNode>(8, StatementType::Read);
    auto node9 = std::make_shared<CFGNode>(9, StatementType::Print);
    auto node10 = std::make_shared<CFGNode>(10, StatementType::Read);
    auto node11 = std::make_shared<CFGNode>(11, StatementType::Read);

    auto testCFGRoot = std::make_shared<CFGNode>();
    auto dummyNode2 = std::make_shared<CFGNode>();
    auto monkeCFGRoot = std::make_shared<CFGNode>();

    /*
    procedure test {
        dummyNode1 start
        while (x < 420) {        1
            while (x > 69) {    2
                x = x + 1;       3
                print x;         4
            }
            read x;              5
            call monke;          6
            if (x == 369) then { 7
                read x;          8
            } else {
                print x;         9
            }
            dummyNode2 for the if else
        }
        read x;                  10
    }
    procedure monke {
        dummyNode3 start
        read y;                  11
    }
    */

    testCFGRoot->insert(node1);
    node1->insert(node2);
    node2->insert(node3);
    node3->insert(node4);
    node4->insert(node2);
    node2->insert(node5);
    node5->insert(node6);
    node6->insert(node7);
    node7->insert(node8);
    node7->insert(node9);
    node8->insert(dummyNode2);
    node9->insert(dummyNode2);
    dummyNode2->insert(node1);
    node1->insert(node10);

    monkeCFGRoot->insert(node11);

    SECTION("Equals Test") {
        SECTION("Simple nodes test") {
            auto nodeTest = std::make_shared<CFGNode>(1, StatementType::While);
            // Same as nodeTest
            auto nodeTest1 = std::make_shared<CFGNode>(1, StatementType::While);
            // Different statement number
            auto nodeTest2 = std::make_shared<CFGNode>(42, StatementType::While);
            // Different StatementType        
            auto nodeTest3 = std::make_shared<CFGNode>(1, StatementType::If);
            // null node 1
            auto nullNode1 = std::make_shared<CFGNode>();
            // null node 2
            auto nullNode2 = std::make_shared<CFGNode>();
            REQUIRE(*nodeTest == *nodeTest1);
            REQUIRE(*nodeTest == *nodeTest1);
            REQUIRE_FALSE(*nodeTest == *nodeTest2);
            REQUIRE_FALSE(*nodeTest == *nodeTest3);
            REQUIRE(*nullNode1 == *nullNode2);
            REQUIRE_FALSE(*nodeTest == *nullNode1);
        }

        SECTION("Children nodes test") {
            /* ================== STMT NODES ================== */
            auto nodeTest1 = std::make_shared<CFGNode>(1, StatementType::While);
            auto childNode1 = std::make_shared<CFGNode>(2, StatementType::Assignment);
            auto childNode2 = std::make_shared<CFGNode>(3, StatementType::Print);
            nodeTest1->insert(childNode1);
            nodeTest1->insert(childNode2);
            // Same statement but different children count
            auto nodeTest2 = std::make_shared<CFGNode>(1, StatementType::While);
            auto childNode3 = std::make_shared<CFGNode>(2, StatementType::Assignment);
            nodeTest2->insert(childNode3);
            // Same statement but different children
            auto nodeTest3 = std::make_shared<CFGNode>(1, StatementType::While);
            auto childNode4 = std::make_shared<CFGNode>(3, StatementType::Assignment);
            auto childNode5 = std::make_shared<CFGNode>(4, StatementType::Print);
            nodeTest3->insert(childNode4);
            nodeTest3->insert(childNode5);
            // Same statement and same children
            auto nodeTest4 = std::make_shared<CFGNode>(1, StatementType::While);
            auto childNode6 = std::make_shared<CFGNode>(2, StatementType::Assignment);
            auto childNode7 = std::make_shared<CFGNode>(3, StatementType::Print);
            nodeTest4->insert(childNode6);
            nodeTest4->insert(childNode7);

            REQUIRE_FALSE(*nodeTest1 == *nodeTest2);
            REQUIRE_FALSE(*nodeTest1 == *nodeTest3);
            REQUIRE(*nodeTest1 == *nodeTest1);
            REQUIRE(*nodeTest1 == *nodeTest4);

            /* =================== NULL NODES =================== */
            // Null node 1
            auto nodeTest5 = std::make_shared<CFGNode>();
            auto childNode8 = std::make_shared<CFGNode>(2, StatementType::Assignment);
            auto childNode9 = std::make_shared<CFGNode>(3, StatementType::Print);
            nodeTest5->insert(childNode8);
            nodeTest5->insert(childNode9);
            // The same null node            
            auto nodeTest6 = std::make_shared<CFGNode>();
            auto childNode10 = std::make_shared<CFGNode>(2, StatementType::Assignment);
            auto childNode11 = std::make_shared<CFGNode>(3, StatementType::Print);
            nodeTest6->insert(childNode10);
            nodeTest6->insert(childNode11);
            // Null node with different children
            auto nodeTest7 = std::make_shared<CFGNode>();
            auto childNode12 = std::make_shared<CFGNode>(2, StatementType::Assignment);
            auto childNode13 = std::make_shared<CFGNode>(3, StatementType::Read);
            nodeTest7->insert(childNode12);
            nodeTest7->insert(childNode13);

            REQUIRE_FALSE(*nodeTest1 == *nodeTest5);
            REQUIRE_FALSE(*nodeTest5 == *nodeTest7);
            REQUIRE(*nodeTest5 == *nodeTest5);
            REQUIRE(*nodeTest5 == *nodeTest6);


        }
    }

    SECTION("Traversal Test") {
        // Testing parent and ancestor relationships for the first procedure in the code
        REQUIRE(testCFGRoot->isParentOf(node1.get()));
        REQUIRE(node1->isParentOf(node2.get()));
        REQUIRE(node2->isParentOf(node3.get()));
        REQUIRE(node3->isParentOf(node4.get()));
        REQUIRE(node4->isParentOf(node2.get()));
        REQUIRE(node2->isParentOf(node5.get()));
        REQUIRE(node5->isParentOf(node6.get()));
        REQUIRE(node6->isParentOf(node7.get()));
        REQUIRE(node7->isParentOf(node8.get()));
        REQUIRE(node7->isParentOf(node9.get()));
        REQUIRE(node8->isParentOf(dummyNode2.get()));
        REQUIRE(node9->isParentOf(dummyNode2.get()));
        REQUIRE(dummyNode2->isParentOf(node1.get()));
        REQUIRE(node1->isParentOf(node10.get()));

        REQUIRE_FALSE(node10->isParentOf(node1.get()));
        REQUIRE_FALSE(node10->isParentOf(node2.get()));
        REQUIRE_FALSE(node10->isParentOf(node3.get()));
        REQUIRE_FALSE(node10->isParentOf(node4.get()));
        REQUIRE_FALSE(node10->isParentOf(node5.get()));
        REQUIRE_FALSE(node10->isParentOf(node6.get()));
        REQUIRE_FALSE(node10->isParentOf(node7.get()));
        REQUIRE_FALSE(node10->isParentOf(node8.get()));
        REQUIRE_FALSE(node10->isParentOf(node9.get()));
        REQUIRE_FALSE(node10->isParentOf(node10.get()));

        REQUIRE_FALSE(node4->isParentOf(node5.get()));
        REQUIRE_FALSE(node8->isParentOf(node7.get()));
        REQUIRE_FALSE(node9->isParentOf(node7.get()));
        REQUIRE_FALSE(node8->isParentOf(node9.get()));
        REQUIRE_FALSE(node9->isParentOf(node8.get()));

        REQUIRE(testCFGRoot->isAncestorOf(node1.get()));
        REQUIRE(node1->isAncestorOf(node2.get()));
        REQUIRE(node2->isAncestorOf(node3.get()));
        REQUIRE(node3->isAncestorOf(node4.get()));
        REQUIRE(node4->isAncestorOf(node2.get()));
        REQUIRE(node2->isAncestorOf(node5.get()));
        REQUIRE(node5->isAncestorOf(node6.get()));
        REQUIRE(node6->isAncestorOf(node7.get()));
        REQUIRE(node7->isAncestorOf(node8.get()));
        REQUIRE(node7->isAncestorOf(node9.get()));
        REQUIRE(node8->isAncestorOf(dummyNode2.get()));
        REQUIRE(node9->isAncestorOf(dummyNode2.get()));
        REQUIRE(dummyNode2->isAncestorOf(node1.get()));
        REQUIRE(node1->isAncestorOf(node10.get()));

        REQUIRE(node1->isAncestorOf(node1.get()));
        REQUIRE(node1->isAncestorOf(node2.get()));
        REQUIRE(node1->isAncestorOf(node3.get()));
        REQUIRE(node1->isAncestorOf(node4.get()));
        REQUIRE(node1->isAncestorOf(node5.get()));
        REQUIRE(node1->isAncestorOf(node6.get()));
        REQUIRE(node1->isAncestorOf(node7.get()));
        REQUIRE(node1->isAncestorOf(node8.get()));
        REQUIRE(node1->isAncestorOf(node9.get()));
        REQUIRE(node1->isAncestorOf(node10.get()));

        REQUIRE(node2->isAncestorOf(node1.get()));
        REQUIRE(node2->isAncestorOf(node2.get()));
        REQUIRE(node2->isAncestorOf(node3.get()));
        REQUIRE(node2->isAncestorOf(node4.get()));
        REQUIRE(node2->isAncestorOf(node5.get()));
        REQUIRE(node2->isAncestorOf(node6.get()));
        REQUIRE(node2->isAncestorOf(node7.get()));
        REQUIRE(node2->isAncestorOf(node8.get()));
        REQUIRE(node2->isAncestorOf(node9.get()));
        REQUIRE(node2->isAncestorOf(node10.get()));

        REQUIRE(node3->isAncestorOf(node1.get()));
        REQUIRE(node3->isAncestorOf(node2.get()));
        REQUIRE(node3->isAncestorOf(node3.get()));
        REQUIRE(node3->isAncestorOf(node4.get()));
        REQUIRE(node3->isAncestorOf(node5.get()));
        REQUIRE(node3->isAncestorOf(node6.get()));
        REQUIRE(node3->isAncestorOf(node7.get()));
        REQUIRE(node3->isAncestorOf(node8.get()));
        REQUIRE(node3->isAncestorOf(node9.get()));
        REQUIRE(node3->isAncestorOf(node10.get()));

        REQUIRE_FALSE(node10->isAncestorOf(node1.get()));
        REQUIRE_FALSE(node10->isAncestorOf(node2.get()));
        REQUIRE_FALSE(node10->isAncestorOf(node3.get()));
        REQUIRE_FALSE(node10->isAncestorOf(node4.get()));
        REQUIRE_FALSE(node10->isAncestorOf(node5.get()));
        REQUIRE_FALSE(node10->isAncestorOf(node6.get()));
        REQUIRE_FALSE(node10->isAncestorOf(node7.get()));
        REQUIRE_FALSE(node10->isAncestorOf(node8.get()));
        REQUIRE_FALSE(node10->isAncestorOf(node9.get()));
        REQUIRE_FALSE(node10->isAncestorOf(node10.get()));
    }

    SECTION("CFG Parsing Test") {
        // Manual AST construction for the code above
        auto ast = ast::makeProgram(
            ast::make<ast::Procedure>(
                "test",
                ast::makeStmts(
                    ast::make<ast::While>(1,
                        ast::make<ast::RelExpr>(
                            ast::RelOp::LT,
                            ast::make<ast::Var>("x"),
                            ast::make<ast::Const>(420)
                        ),
                        ast::makeStmts(
                            ast::make<ast::While>(
                                2,
                                ast::make<ast::RelExpr>(
                                    ast::RelOp::GT,
                                    ast::make<ast::Var>("x"),
                                    ast::make<ast::Const>(69)
                                ),
                                ast::makeStmts(
                                    ast::make<ast::Assign>(
                                        3,
                                        ast::make<ast::Var>("x"), 
                                        ast::make<ast::BinExpr>(
                                            ast::BinOp::PLUS, 
                                            ast::make<ast::Var>("x"), 
                                            ast::make<ast::Const>(1)
                                        )
                                    ),
                                    ast::make<ast::Print>(4, ast::make<ast::Var>("x"))
                                )
                            ),
                            ast::make<ast::Read>(5, ast::make<ast::Var>("x")),
                            ast::make<ast::Call>(6, "monke"),
                            ast::make<ast::If>(
                                7,
                                ast::make<ast::RelExpr>(
                                    ast::RelOp::EQ,
                                    ast::make<ast::Var>("x"),
                                    ast::make<ast::Const>(369)
                                ),
                                ast::makeStmts(
                                    ast::make<ast::Read>(8, ast::make<ast::Var>("x"))
                                ),
                                ast::makeStmts(
                                    ast::make<ast::Print>(9, ast::make<ast::Var>("x"))
                                )
                            )
                        )
                    ),
                    ast::make<ast::Read>(10, ast::make<ast::Var>("x"))
                )
            ),
            ast::make<ast::Procedure>(
                "monke", 
                ast::makeStmts(
                    ast::make<ast::Read>(11, ast::make<ast::Var>("y"))
                )
            )
        );

        CFGExtractor extractor;
        auto result = extractor.extract(ast.get());
        REQUIRE(*result.cfgs.at("test") == *testCFGRoot);
        REQUIRE(*result.cfgs.at("monke") == *monkeCFGRoot);
    }

    SECTION("Test Relationship Info") {
        // We add Modifies info for Read and Call, and both Modifies and Uses info for Assign.
        // We will test for all possible statement types.
        auto ast = sp::parser::parse(
            R"(
            procedure test {
                while (x == 1) {
                    print y;
                }
                if (y == 2) then {
                    read x;
                } else {
                    call monke;
                }
            }
            procedure monke {
                z = x + y;
            }
            )"
        );
        CFGExtractor extractor;
        auto result = extractor.extract(ast.get());
        // There should be two procedures.
        // 1st procedure:
        auto proc1 = result.cfgs.at("test");

        // Line 1: while statement
        auto stmt1 = proc1->getChildren().at(0).lock();
        REQUIRE(stmt1->modifies.empty());
        REQUIRE(stmt1->uses.empty());

        // Line 2: print statement
        auto stmt2 = stmt1->getChildren().at(0).lock();  // print node is the first child of while node
        REQUIRE(stmt2->modifies.empty());
        REQUIRE(stmt2->uses.empty());

        // Line 3: if statement
        auto stmt3 = stmt1->getChildren().at(1).lock();  // if node is the second child of while node
        REQUIRE(stmt3->modifies.empty());
        REQUIRE(stmt3->uses.empty());

        // Line 4: read statement
        auto stmt4 = stmt3->getChildren().at(0).lock();  // statements inside then block of if first
        auto stmt4lo = STMT_LO(4, StatementType::Read);
        // Should contain modifies
        std::unordered_set<VAR_NAME> expectedVars1 ({
            VAR_NAME("x")
        });
        REQUIRE(stmt4->modifies == expectedVars1);
        REQUIRE(stmt4->uses.empty());

        // Line 5: call statement
        auto stmt5 = stmt3->getChildren().at(1).lock();  // statements inside the else block of if
        auto stmt5lo = STMT_LO(5, StatementType::Call);
        // Should contain modifies
        std::unordered_set<VAR_NAME> expectedVars2 ({
            VAR_NAME("z")
        });
        REQUIRE(stmt5->modifies == expectedVars2);
        REQUIRE(stmt5->uses.empty());

        // 2nd procedure:
        auto proc2 = result.cfgs.at("monke");
        // Line 6: assignment statement
        auto stmt6 = proc2->getChildren().at(0).lock();
        auto stmt6lo = STMT_LO(6, StatementType::Assignment);
        // Should contain modifies and uses
        // modifies
        std::unordered_set<VAR_NAME> expectedVars3 ({
            VAR_NAME("z")
        });
        // uses
        std::unordered_set<VAR_NAME> expectedVars4 ({
            VAR_NAME("x"), VAR_NAME("y")
        });
        REQUIRE(stmt6->modifies == expectedVars3);
        REQUIRE(stmt6->uses == expectedVars4);
    }
}
}  // namespace cfg
}  // namespace sp
