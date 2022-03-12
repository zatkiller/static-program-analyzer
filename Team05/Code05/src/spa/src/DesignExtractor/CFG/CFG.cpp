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
    return (this->stmt == o.stmt) && (this->children == o.children);
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
    return dfs(this, other, reached);
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
    auto newNode = std::make_shared<CFGNode>(node.getStmtNo(), StatementType::If);
    lastVisited->insert(newNode);
    lastVisited = newNode;
    isIf++;
};

void CFGExtractor::visit(const ast::While& node) {
    auto newNode = std::make_shared<CFGNode>(node.getStmtNo(), StatementType::While);
    lastVisited->insert(newNode);
    lastVisited = newNode;
    isWhile = true;
}

void CFGExtractor::visit(const ast::Read& node) {
    auto newNode = std::make_shared<CFGNode>(node.getStmtNo(), StatementType::Read);
    lastVisited->insert(newNode);
    lastVisited = newNode;
}

void CFGExtractor::visit(const ast::Print& node) {
    auto newNode = std::make_shared<CFGNode>(node.getStmtNo(), StatementType::Print);
    lastVisited->insert(newNode);
    lastVisited = newNode;
}

void CFGExtractor::visit(const ast::Assign& node) {
    auto newNode = std::make_shared<CFGNode>(node.getStmtNo(), StatementType::Assignment);
    lastVisited->insert(newNode);
    lastVisited = newNode;
}

void CFGExtractor::visit(const ast::Call& node) {
    auto newNode = std::make_shared<CFGNode>(node.getStmtNo(), StatementType::Call);
    lastVisited->insert(newNode);
    lastVisited = newNode;
}

void CFGExtractor::enterContainer(std::variant<int, std::string> containerId) {
    if (isIf) {
        if (isIf > 2) {
            isIf = 0;
            return;
        }
        isIf++;
        // TODO
    }  else if (isWhile) {
        // TODO
    }
}

void CFGExtractor::exitContainer() {
    // TODO
};

}  // namespace cfg
}  // namespace sp
