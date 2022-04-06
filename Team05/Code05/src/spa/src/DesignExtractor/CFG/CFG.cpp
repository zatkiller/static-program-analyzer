#include <unordered_set>

#include "CFG.h"
#include "DesignExtractor/RelationshipExtractor/ModifiesExtractor.h"
#include "DesignExtractor/RelationshipExtractor/UsesExtractor.h"
#include "logging.h"

#define DEBUG_LOG Logger(Level::DEBUG) << "CFGExtractor.cpp Extracted "

namespace sp {
namespace cfg {

void CFGNode::insert(std::shared_ptr<CFGNode> child) {
    children.push_back(child);
}

bool checkChildrenEquality(
    const CFGNode* curr, 
    const CFGNode* other, 
    std::unordered_set<const CFGNode*> reached) {
    auto currChildren = curr->getChildren();
    auto otherChildren = other->getChildren();
    reached.insert(curr);
    reached.insert(other);
    if (currChildren.size() != otherChildren.size()) {
        return false;
    }
    bool isEqual = true;
    for (auto c : currChildren) {
        // if the child has already been traversed, we ignore
        if (reached.find(c.get()) == reached.end()) {
            continue;
        }
        bool isMatch = false;
        for (auto o : otherChildren) {
            // if you find a matching statement, we can recurse down these two nodes
            if (c.get()->stmt == o.get()->stmt) {
                isMatch = checkChildrenEquality(c.get(), o.get(), reached);
                break;
            }
        }
        isEqual = isEqual && isMatch;
        // if the previously checks nodes where not equal, terminate and return false
        if (!isEqual) {
            return isEqual;
        }
    }
    // after successfully comparing and recursing down the children, return
    return isEqual;
}

bool CFGNode::operator==(CFGNode const& o) const {
    std::unordered_set<const CFGNode*> reached;
    return this->stmt == o.stmt && checkChildrenEquality(this, &o, reached);
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

using EntrySet = std::set<sp::design_extractor::Entry>;

ContentToVarMap createContentVarMap(const EntrySet &entrySet) {
    ContentToVarMap result;
    for (auto entry : entrySet) {
        auto relationship = std::get<design_extractor::Relationship>(entry);
        Content key, val;
        std::tie (std::ignore, key, val) = relationship;
        if (result.find(key) != result.end()) {
            result.at(key).insert(std::get<VAR_NAME>(val));
        } else {
            std::unordered_set<VAR_NAME> newSet;
            newSet.insert(std::get<VAR_NAME>(val));
            result[key] = newSet;
        }
    }
    return result;
}

VAR_NAMES filterContentVarMap(const ContentToVarMap &contentToVarMap, const Content &content) {
    VAR_NAMES result;
    auto entry = contentToVarMap.find(content);
    if (entry != contentToVarMap.end()) {
        result = entry->second;
    }
    return result;
}

void CFGExtractor::visit(const ast::Procedure& node) {
    // create a dummy node as a start node for each procedure
    auto startNode = std::make_shared<CFGNode>();
    procNameAndRoot.insert(
        std::pair<std::string, std::shared_ptr<CFGNode>>(node.getName(), startNode)
    );
    lastVisited = startNode;
}

void CFGExtractor::visit(const ast::If& node) {
    containerCount += 2;
    auto newNode = std::make_shared<CFGNode>(node.getStmtNo(), StatementType::If);
    lastVisited->insert(newNode);
    lastVisited = newNode;
    // Whenever you enter an If/Else stmtLst, parent node should be the If node.
    enterBucket(newNode);
    // When you exit an If/Else stmtLst, the last statement should point to a dummy exit node.
    auto dummyExitNode = std::make_shared<CFGNode>();
    exitReference.insert_or_assign(currentDepth, dummyExitNode);
}

void CFGExtractor::visit(const ast::While& node) {
    containerCount++;
    auto newNode = std::make_shared<CFGNode>(node.getStmtNo(), StatementType::While);
    lastVisited->insert(newNode);
    lastVisited = newNode;
    // When you enter a While stmtLst, the parent node should be the While node.
    enterBucket(newNode);
    // When you exit a While container, the last statement should point to the While node.
    exitReference.insert_or_assign(currentDepth, newNode);
}

void CFGExtractor::visit(const ast::Read& node) {
    auto newNode = std::make_shared<CFGNode>(node.getStmtNo(), StatementType::Read);
    // inserting Modifies relationship info for Read stmt CFGNode
    STMT_LO stmt = STMT_LO(node.getStmtNo(), StatementType::Read);
    newNode->modifies = filterContentVarMap(modifiesMap, stmt);

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
    // inserting Modifies relationship info for Assign stmt CFGNode
    STMT_LO stmt = STMT_LO(node.getStmtNo(), StatementType::Assignment);
    newNode->modifies = filterContentVarMap(modifiesMap, stmt);
    newNode->uses = filterContentVarMap(usesMap, stmt);

    lastVisited->insert(newNode);
    lastVisited = newNode;
    enterBucket(newNode);
}

void CFGExtractor::visit(const ast::Call& node) {
    auto newNode = std::make_shared<CFGNode>(node.getStmtNo(), StatementType::Call);
    // inserting Modifies relationship info for Call stmt CFGNode
    STMT_LO stmt = STMT_LO(node.getStmtNo(), StatementType::Call);
    newNode->modifies = filterContentVarMap(modifiesMap, stmt);

    lastVisited->insert(newNode);
    lastVisited = newNode;
    enterBucket(newNode);
}

void CFGExtractor::enterContainer(std::variant<int, std::string> containerId) {
    // if it is an If/While statement list
    if (containerCount) {
        // the lastVisited node should be the If/While statement itself
        lastVisited = bucket.at(currentDepth);
    }
    currentDepth++;
}

void CFGExtractor::exitContainer() {
    currentDepth--;
    // if it is an If/While statement list
    if (containerCount) {
        containerCount--;
        /* 
        the lastVisited node should point to
        1. If: dummy exit node
        2. While: While statement itself
        */
        lastVisited->insert(exitReference.at(currentDepth));
        lastVisited = exitReference.at(currentDepth);
    }
}

std::map<std::string, std::shared_ptr<CFGNode>> CFGExtractor::extract(ast::ASTNode* node) {
    auto modifiesEntrySet = design_extractor::ModifiesExtractor().extract(node);
    modifiesMap = createContentVarMap(modifiesEntrySet);
    auto usesEntrySet = design_extractor::UsesExtractor().extract(node);
    usesMap = createContentVarMap(usesEntrySet);
    node->accept(this);
    return procNameAndRoot;
}

}  // namespace cfg
}  // namespace sp
