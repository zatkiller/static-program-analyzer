#pragma once

#include <vector>
#include <memory>

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
    explicit CFGNode(int stmtNo) : stmt(STMT_LO(stmtNo)) {}

    void insert(std::shared_ptr<CFGNode> child);
    bool isParentOf(CFGNode* other);
    bool isAncestorOf(CFGNode* other);
    
    bool operator==(CFGNode const& o) const;

    std::vector<std::shared_ptr<CFGNode>> getChildren() { return children; }
};

// TODO(NayLin-H99): AST to CFG construction
// class CFGExtractor: public TreeWalker {
//     void visit(const ast::Program& node) override;
//     void visit(const ast::Procedure& node) override;
//     void visit(const ast::StmtLst& node) override;
//     void visit(const ast::If& node) override;
//     void visit(const ast::While& node) override;
//     void visit(const ast::Read& node) override;
//     void visit(const ast::Print& node) override;
//     void visit(const ast::Assign& node) override;
//     void visit(const ast::Call&) override;
//     void enterContainer(std::variant<int, std::string> containerId) override;
//     void exitContainer() override;
// }
}  // namespace cfg
}  // namespace sp
