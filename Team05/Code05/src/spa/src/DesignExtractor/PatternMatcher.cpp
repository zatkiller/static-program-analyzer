#include "PatternMatcher.h"

namespace sp {
namespace design_extractor {
struct ExprFlattener : public TreeWalker {
    std::list<std::reference_wrapper<const ast::ASTNode>> nodes;
    void visit(const ast::Var& node) override {
        nodes.emplace_back(node);
    };
    void visit(const ast::Const& node) override {
        nodes.emplace_back(node);
    };
    void visit(const ast::BinExpr& node) override {
        nodes.emplace_back(node);
    };
    void visit(const ast::RelExpr& node) override {
        nodes.emplace_back(node);
    };
    void visit(const ast::CondBinExpr& node) override {
        nodes.emplace_back(node);
    };
    void visit(const ast::NotCondExpr& node) override {
        nodes.emplace_back(node);
    };
};


// Helper method to flatten any expression tree to a list representation.
std::list<std::reference_wrapper<const ast::ASTNode>> flatten(ast::Expr *root) {
    auto flattener = std::make_shared<ExprFlattener>();
    root->accept(flattener);
    return flattener->nodes;
}


// Checks and returns true if the needle list is a sublist of the haystack list.
bool isSublist(
    std::list<std::reference_wrapper<const ast::ASTNode>> haystack,
    std::list<std::reference_wrapper<const ast::ASTNode>> needle
    ) {
    auto haystackIter = haystack.begin();
    auto needleIter = needle.begin();

    if (haystack.size() < needle.size()) {
        return false;
    }

    while(haystackIter != haystack.end() && needleIter != needle.end()) {
        if (haystackIter->get() == needleIter->get()) {
            needleIter++;
        } else {
            needleIter = needle.begin();
        }
        haystackIter++;
    }

    return needleIter == needle.end();
}

class AssignmentPatternExtractor : public TreeWalker {
private:
    PatternParam lhs, rhs;
    bool isStrict;  // Not in use yet.

public:
    std::list<std::reference_wrapper<const ast::Assign>> nodes;
    bool isMatch(const ast::Assign& node) {
        // === Check lHS constraint ===
        if (lhs != std::nullopt && node.getLHS()->getVarName() != lhs.value()) {
            return false;
        }

        // === Check RHS constraint ===

        // case 1: RHS is unconstrained and it's LHS constraint has passed. return true.
        if (rhs == std::nullopt) {
            return true;
        }

        // case 2: RHS is constrained

        // Parse RHS to AST expression.
        auto tokens = sp::parser::Lexer(rhs.value()).getTokens();
        auto expr = sp::parser::expr_parser::parse(tokens);

        // Flatten the trees to lists by walking it in preorder.
        auto assignList = flatten(node.getRHS());
        auto exprList = flatten(expr.get());

        // check if RHS expression list is a sublist of assignment expression list
        return isSublist(assignList, exprList);
    }

    /**
     * @brief Construct a new Assignment Pattern Extractor object
     * 
     * @param lhs Optional constraint for the left hand side of assignment. Give nullopt if wildcard or declaration.
     * @param rhs Optional constraint for the right hand side of assignment. Give nullopt if wildcard or declaration.
     * @param isStrict Deteremines if the matching is strict. If it's strict, exact match is expected. Defaults to false.
     */
    AssignmentPatternExtractor(PatternParam lhs, PatternParam rhs, bool isStrict = false) : 
        lhs(lhs),
        rhs(rhs), 
        isStrict(isStrict) {}

    void visit(const ast::Assign& node) override {
        if (isMatch(node)) {
            nodes.emplace_back(node);
        }
    };
};

AssignPatternReturn extractAssign(ast::ASTNode *root, PatternParam lhs, PatternParam rhs) {
    auto ape = std::make_shared<AssignmentPatternExtractor>(lhs, rhs);
    root->accept(ape);
    return ape->nodes;
}
}  // namespace design_extractor
}  // namespace sp
