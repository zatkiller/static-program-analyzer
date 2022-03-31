#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>

#include "PKB/PKBField.h"
#include "DesignExtractor/TreeWalker.h"

namespace sp {
namespace cfg {



class CFGNode;
using PROC_CFG_MAP = std::map<std::string, std::shared_ptr<cfg::CFGNode>>;
using VAR_NAMES = std::unordered_set<VAR_NAME>;
using ContentToVarMap = std::unordered_map<Content, VAR_NAMES>;

/**
 * @brief Class to encapsulate a single CFGNode.
 * @details children is a list of the direct children of the current node.
 * Dummy node will have STMT_LO as nullopt.
 */
class CFGNode {
private:
    std::vector<std::shared_ptr<CFGNode>> children;
public:
    std::optional<STMT_LO> stmt;
    VAR_NAMES modifies;  // information on modifies relationship for current node
    VAR_NAMES uses;  // information on uses relationship relationship for current node
    CFGNode () : stmt(std::nullopt) {}
    CFGNode(int stmtNo, StatementType stmtType) : stmt(STMT_LO(stmtNo, stmtType)) {}

    void insert(std::shared_ptr<CFGNode> child);
    bool isParentOf(CFGNode* other);
    bool isAncestorOf(CFGNode* other);
    
    bool operator==(CFGNode const& o) const;

    std::vector<std::shared_ptr<CFGNode>> getChildren() const { return children; }
};

using Depth = int;
using Bucket = std::unordered_map<Depth, std::shared_ptr<CFGNode>>;
using VarName = std::string;
/**
 * @brief Class that extracts a CFG (CFGNode) from AST (ASTNode).
 * @details The extractor does a pre-order walk of the tree. 
 * The root of the CFG for each procedure is always a dummy node.
 * Entrance and exit nodes for if and while statements at different nesting levels are kept track of 
 * with two "buckets", bucket and exitReference.
 * bucket keeps track of the starting node of each stmtLst.
 * exitReference keeps track of the exit node of each stmtLst.
 * The If node will always split into two different paths of execution and converge into a dummy node,
 * and subsequent statements will point from the dummy node.
 * The While node will always lead down the path of execution and point back to the parent While node,
 * and subsequent statements will point from the While node.
 * The extract() method will return a map of procedure names to their respective CFGs.
 */
class CFGExtractor: public design_extractor::TreeWalker {
private:
    std::map<std::string, std::shared_ptr<CFGNode>> procNameAndRoot;
    std::shared_ptr<CFGNode> lastVisited;
    int containerCount = 0;  // keeps track of # of containers/stmtLst to enter for if or while stmts
    Bucket bucket, exitReference;  // keeps track parent and exit CFGnodes at different nesting levels
    Depth currentDepth = 0;  // keeps track of the depth of the current statement in the stmtLst
    ContentToVarMap modifiesMap, usesMap;  // reference for adding modifies and uses information to the CFG
    void enterBucket(std::shared_ptr<CFGNode> node) {
        bucket.insert_or_assign(currentDepth, node);
    }
public:
    void visit(const ast::Procedure& node) override;
    void visit(const ast::If& node) override;
    void visit(const ast::While& node) override;
    void visit(const ast::Read& node) override;
    void visit(const ast::Print& node) override;
    void visit(const ast::Assign& node) override;
    void visit(const ast::Call& node) override;
    void enterContainer(std::variant<int, std::string> containerId) override;
    void exitContainer() override;
    std::map<std::string, std::shared_ptr<CFGNode>> extract(ast::ASTNode* node);
};
}  // namespace cfg
}  // namespace sp
