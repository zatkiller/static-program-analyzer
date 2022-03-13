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
    auto node11 = std::make_shared<CFGNode>(10, StatementType::Read);

    auto dummyNode1 = std::make_shared<CFGNode>();
    auto dummyNode2 = std::make_shared<CFGNode>();
    auto dummyNode3 = std::make_shared<CFGNode>();

    /*
    procedure test {
        dummyNode1 start
        while (x < 420) {        1
            while (x > 69)) {    2
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

    dummyNode1->insert(node1);
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

    dummyNode3->insert(node11);

    SECTION("Equals Test") {
        // Same as node1
        auto nodeTest = std::make_shared<CFGNode>(1, StatementType::While);
        nodeTest->insert(node2);
        nodeTest->insert(node10);
        // Different statement number
        auto nodeTest1 = std::make_shared<CFGNode>(420, StatementType::While);
        nodeTest1->insert(node2);
        nodeTest1->insert(node10);
        // Different StatementType        
        auto nodeTest2 = std::make_shared<CFGNode>(1, StatementType::If);
        nodeTest2->insert(node2);
        nodeTest2->insert(node10);
        REQUIRE(*node1 == *nodeTest);
        REQUIRE_FALSE(*node1 == *nodeTest1);
        REQUIRE_FALSE(*node1 == *nodeTest2);
        // auto dummyNodeTest = std::make_shared<CFGNode>();
    }

    SECTION("Traversal Test") {
        // Testing parent and ancestor relationships for the first procedure in the code
        REQUIRE(dummyNode1->isParentOf(node1.get()));
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

        REQUIRE(dummyNode1->isAncestorOf(node1.get()));
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
                    ast::make<ast::Read>(1, ast::make<ast::Var>("y"))
                )
            )
        );

        CFGExtractor extractor;
        auto result = extractor.extract(ast.get());
        
        REQUIRE(*result.find("test")->second == *dummyNode1);
        REQUIRE(*result.find("monke")->second == *dummyNode3);
    }
}
}  // namespace cfg
}  // namespace sp
