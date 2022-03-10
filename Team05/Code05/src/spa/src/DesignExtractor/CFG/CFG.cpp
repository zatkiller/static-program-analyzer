#include <unordered_set>

#include "CFG.h"

namespace sp {
namespace cfg {

void CFGNode::insert(std::shared_ptr<CFGNode> child) {
    children.push_back(child);
}

bool CFGNode::operator==(CFGNode const& o) const {
    return (this->stmt == o.stmt);
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

// TODO(NayLin-H99): AST to CFG construction
// void visit(const ast::Program& node) override {}; 
// void visit(const ast::Procedure& node) override {};
// void visit(const ast::StmtLst& node) override {};
// void visit(const ast::If& node) override {};
// void visit(const ast::While& node) override {};
// void visit(const ast::Read& node) override {};
// void visit(const ast::Print& node) override {};
// void visit(const ast::Assign& node) override {};
// void visit(const ast::Call&) override {};
// void enterContainer(std::variant<int, std::string> containerId) override {};
// void exitContainer() override {};
}  // namespace cfg
}  // namespace sp
