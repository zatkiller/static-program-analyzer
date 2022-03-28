#include <functional>

#include "PatternMatcher.h"
#include "Parser/Lexer.h"
#include "Parser/Parser.h"
#include "PKB/PKBField.h"
#include "DesignExtractor.h"
#include "DesignExtractor/EntityExtractor/EntityExtractor.h"

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

/**
 * @brief Collects a single type of node in the ast tree
 * 
 * @tparam T the type of node that needs to be collected.
 */
template <typename T>
struct SingleCollector : public TreeWalker {
    std::list<std::reference_wrapper<const T>> nodes;
    void visit(const T& node) override {
        nodes.emplace_back(node);
    };
};

// Helper method to flatten any expression tree to a list representation.
std::list<std::reference_wrapper<const ast::ASTNode>> flatten(ast::Expr *root) {
    auto flattener = std::make_unique<ExprFlattener>();
    root->accept(flattener.get());
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

template<typename T>
std::list<std::reference_wrapper<const T>> extractPattern(ast::ASTNode *root, std::function<bool(const T&)> isMatch) {
    SingleCollector<T> collector;
    root->accept(&collector);
    std::list<std::reference_wrapper<const T>> matched;
    for (auto stmt : collector.nodes) {
        if (isMatch(stmt)) {
            matched.emplace_back(stmt);
        }
    }
    return matched;
}

template<typename T>
std::function<bool(const T&)> makeCondPredicate(PatternParam var) {
    return [var](const T& node) {
        if (!var.has_value()) {
            return true;
        }

        SingleCollector<ast::Var> varCollector;
        node.getCondExpr()->accept(&varCollector);
        for (auto condVar : varCollector.nodes) {
            if (condVar.get().getVarName() == var.value()) {
                return true;
            }
        }
        return false;
    };
}

std::function<bool(const ast::Assign&)> makeAssignPredicate(PatternParam lhs, PatternParam rhs, bool isStrict) {
    return [lhs, rhs, isStrict](const ast::Assign& node) {
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

        // If it is strict, RHS must be an exact match.
        if (isStrict) {
            return *node.getRHS() == *expr;
        }

        // If it is not strict, check sublist matching.

        // Flatten the trees to lists by walking it in preorder.
        auto assignList = flatten(node.getRHS());
        auto exprList = flatten(expr.get());

        // check if RHS expression list is a sublist of assignment expression list
        return isSublist(assignList, exprList);
    };
}

IfPatternReturn extractIf(ast::ASTNode *root, PatternParam var) {
    return extractPattern<ast::If>(root, makeCondPredicate<ast::If>(var));
}

WhilePatternReturn extractWhile(ast::ASTNode *root, PatternParam var) {
    return extractPattern<ast::While>(root, makeCondPredicate<ast::While>(var));
}

AssignPatternReturn extractAssign(ast::ASTNode *root, PatternParam lhs, PatternParam rhs, bool isStrict) {
    return extractPattern<ast::Assign>(root, makeAssignPredicate(lhs, rhs, isStrict));
};

}  // namespace design_extractor
}  // namespace sp
