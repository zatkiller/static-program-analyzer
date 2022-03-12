#pragma once

#include <vector>
#include <memory>
#include <unordered_map>

#include "PKB/PKBField.h"
#include "DesignExtractor/Extractor.h"

namespace sp {
namespace cfg {
class CFGNode;
/**
 * @brief Class to encapsulate a single CFGNode.
 * @details children is a list of the direct children of the current node.
 * Dummy node will have STMT_LO as nullopt.
 */
class CFGNode {
private:
    std::optional<STMT_LO> stmt;
    std::vector<std::shared_ptr<CFGNode>> children;
public:
    CFGNode () : stmt(std::nullopt) {}
    explicit CFGNode(int stmtNo, StatementType stmtType) : stmt(STMT_LO(stmtNo, stmtType)) {}

    void insert(std::shared_ptr<CFGNode> child);
    bool isParentOf(CFGNode* other);
    bool isAncestorOf(CFGNode* other);
    
    bool operator==(CFGNode const& o) const;

    std::vector<std::shared_ptr<CFGNode>> getChildren() { return children; }
};

using Depth = int;
using Bucket = std::unordered_map<Depth, std::shared_ptr<CFGNode>>;

class CFGExtractor: public design_extractor::TreeWalker {
private:
    std::map<std::string, std::shared_ptr<CFGNode>> procNameAndRoot;
    std::shared_ptr<CFGNode> lastVisited;
    bool isWhile;  // to indicate if we are currently in a while loop
    int isIf;  // to take 0, 1 (allows 1st enterContainer) or 2 (allows 2nd enterContainer)
    Bucket bucket;
    Depth currentDepth = 0;
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
};
}  // namespace cfg
}  // namespace sp
