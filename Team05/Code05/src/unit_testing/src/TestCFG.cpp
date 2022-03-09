#include <vector>
#include <memory>
#include <unordered_set>

#include "catch.hpp"
#include "DesignExtractor/CFG/CFG.h"

#define TEST_LOG Logger() << "TestDesignExtractor.cpp "

namespace sp {
namespace cfg {
TEST_CASE("CFG Test") {
    // Construction test
    auto node1 = std::make_shared<CFGNode>(1);
    auto node2 = std::make_shared<CFGNode>(2);
    auto node3 = std::make_shared<CFGNode>(3);
    auto node4 = std::make_shared<CFGNode>(4);
    auto node5 = std::make_shared<CFGNode>(5);
    auto node6 = std::make_shared<CFGNode>(6);
    auto node7 = std::make_shared<CFGNode>(7);
    auto dummyNode = std::make_shared<CFGNode>();

    SECTION("Insertion and Traversal Test") {
        /*
        1\
        | \
        2  \
       /|\   \
      / 3 |  |
      | | |  |
      | 4/   |
      |      /
       \5  /
        | /
        6/
        */
        node1->insert(node2);
        node2->insert(node3);
        node3->insert(node4);
        node4->insert(node2);
        node2->insert(node5);
        node5->insert(node6);
        node6->insert(node1);

        REQUIRE(node1->isParentOf(node2.get()));
        REQUIRE(node2->isParentOf(node3.get()));
        REQUIRE(node3->isParentOf(node4.get()));
        REQUIRE_FALSE(node4->isParentOf(node5.get()));
        REQUIRE(node4->isParentOf(node2.get()));
        REQUIRE(node2->isParentOf(node5.get()));
        REQUIRE(node5->isParentOf(node6.get()));
        REQUIRE(node6->isParentOf(node1.get()));

        REQUIRE(node1->isAncestorOf(node1.get()));
        REQUIRE_FALSE(node7->isAncestorOf(node6.get()));
        REQUIRE(node2->isAncestorOf(node2.get()));
        REQUIRE(node2->isAncestorOf(node4.get()));
        REQUIRE(node2->isAncestorOf(node1.get()));
        REQUIRE(node3->isAncestorOf(node1.get()));
        REQUIRE(node4->isAncestorOf(node1.get()));
        REQUIRE(node6->isAncestorOf(node4.get()));
        REQUIRE(node1->isAncestorOf(node5.get()));
        REQUIRE(node1->isAncestorOf(node6.get()));
    }
}
}  // namespace cfg
}  // namespace sp