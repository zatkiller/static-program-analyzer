#pragma once

#include <unordered_map>
#include <vector>

#include "TreeWalker.h"
#include "Parser/ast.h"

namespace sp {
namespace design_extractor {


class CallGraph : public TreeWalker {
public:
    using AdjacencyList = std::unordered_map<std::string, std::vector<std::string>>;
    explicit CallGraph(const ast::ASTNode* root) {
        root->accept(this);
    }

    virtual void visit(const ast::Procedure& node) {
        // unified starting point 1 which is a procedure name that cannot exist.
        callGraph["1"].push_back(node.getName());
        currentProc = node.getName();
        procMap[currentProc] = &node;
    }

    virtual void visit(const ast::Call& node) {
        callGraph[currentProc].push_back(node.getName());
    }

    const AdjacencyList getCallGraph() {
        return callGraph;
    }

    std::unordered_map<std::string, const ast::Procedure*> getProcMap() {
            return procMap;
    }

protected:
    std::string currentProc = "";
    AdjacencyList callGraph;
    std::unordered_map<std::string, const ast::Procedure*> procMap;
};
}  // namespace design_extractor
}  // namespace sp
