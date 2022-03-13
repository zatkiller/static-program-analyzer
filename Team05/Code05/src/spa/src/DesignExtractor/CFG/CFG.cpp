#include <unordered_set>

#include "CFG.h"
#include "logging.h"

#define DEBUG_LOG Logger(Level::DEBUG) << "CFGExtractor.cpp Extracted "

namespace sp {
namespace cfg {

void CFGNode::insert(std::shared_ptr<CFGNode> child) {
    children.push_back(child);
}

bool CFGNode::operator==(CFGNode const& o) const {
    return this->stmt == o.stmt;
}

bool CFGNode::isParentOf(CFGNode* other) {
    for (auto c : this->children) {
        if (*c == *other) {
            return true;
        }
    }
    return false;
}

bool dfs(
    CFGNode* curr,
    CFGNode* other, 
    std::unordered_set<CFGNode*> reached) {
    if (*curr == *other) {
        return true;
    } else {
        for (auto c : curr->getChildren()) {
            if (reached.find(c.get()) != reached.end()) {
                continue;
            }
            reached.insert(curr);
            if (dfs(c.get(), other, reached)) {
                return true;
            }
        }
        return false;
    }
}

bool CFGNode::isAncestorOf(CFGNode* other) {
    std::unordered_set<CFGNode*> reached;
    for (auto c : this->children) {
        if (dfs(c.get(), other, reached)) {
            return true;
        }
    }
    return false;
}

void CFGExtractor::visit(const ast::Procedure& node) {
    // create a dummy node as a start node for each procedure
    auto startNode = std::make_shared<CFGNode>();
    procNameAndRoot.insert(
        std::pair<std::string, std::shared_ptr<CFGNode>>(node.getName(), startNode)
    );
    lastVisited = startNode;
};

void CFGExtractor::visit(const ast::If& node) {
    containerCount += 2;
    auto newNode = std::make_shared<CFGNode>(node.getStmtNo(), StatementType::If);
    lastVisited->insert(newNode);
    lastVisited = newNode;
    // Whenever you enter an if ... then or else stmtLst, parent node should be the If node.
    enterBucket(newNode);
    // When you exit an if stmtLst, you need to point to a dummy exit node
    auto dummyExitNode = std::make_shared<CFGNode>();
    exitReference.insert_or_assign(currentDepth, dummyExitNode);
};

void CFGExtractor::visit(const ast::While& node) {
    containerCount++;
    auto newNode = std::make_shared<CFGNode>(node.getStmtNo(), StatementType::While);
    lastVisited->insert(newNode);
    lastVisited = newNode;
    // When you enter a While stmtLst, the parent node should be the While node.
    enterBucket(newNode);
    // When you exit a While container, you need to point back to the While node.
    exitReference.insert_or_assign(currentDepth, newNode);
}

void CFGExtractor::visit(const ast::Read& node) {
    auto newNode = std::make_shared<CFGNode>(node.getStmtNo(), StatementType::Read);
    lastVisited->insert(newNode);
    lastVisited = newNode;
    enterBucket(newNode);
}

void CFGExtractor::visit(const ast::Print& node) {
    auto newNode = std::make_shared<CFGNode>(node.getStmtNo(), StatementType::Print);
    lastVisited->insert(newNode);
    lastVisited = newNode;
    enterBucket(newNode);
}

void CFGExtractor::visit(const ast::Assign& node) {
    auto newNode = std::make_shared<CFGNode>(node.getStmtNo(), StatementType::Assignment);
    lastVisited->insert(newNode);
    lastVisited = newNode;
    enterBucket(newNode);
}

void CFGExtractor::visit(const ast::Call& node) {
    auto newNode = std::make_shared<CFGNode>(node.getStmtNo(), StatementType::Call);
    lastVisited->insert(newNode);
    lastVisited = newNode;
    enterBucket(newNode);
}

void CFGExtractor::enterContainer(std::variant<int, std::string> containerId) {
    if (containerCount) {
        lastVisited = bucket.find(currentDepth)->second;
    }
    currentDepth++;
}

void CFGExtractor::exitContainer() {
    currentDepth--;
    if (containerCount) {
        containerCount--;
        lastVisited->insert(exitReference.find(currentDepth)->second);
        lastVisited = exitReference.find(currentDepth)->second;
    }
};

std::map<std::string, std::shared_ptr<CFGNode>> CFGExtractor::extract(ast::ASTNode* node) {
    node->accept(this);
    return procNameAndRoot;
}

}  // namespace cfg
}  // namespace sp
