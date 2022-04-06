#include <functional>

#include "DesignExtractor.h"
#include "DesignExtractor/EntityExtractor/EntityExtractor.h"
#include "Parser/Lexer.h"
#include "Parser/Parser.h"
#include "PatternMatcher.h"
#include "PKB/PKBField.h"


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
 * The collected nodes will be stored in the struct as a field nodes.
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

/**
 * @brief A general pattern extractor that returns a list of reference of target statement node of type T
 * 
 * The general pattern extractor that collects all the statement of target type and check them one by one
 * to see if they pass the constraint predicate. The predicate should be partially applied with the constraints
 * so that it only require the statement node to return the result.
 * 
 * @tparam T the target statement type. It can be ast::Assign / ast::If / ast::While
 * @param root the root of the ast tree.
 * @param isMatch the predicate that checks statement of type T
 * @return std::list<std::reference_wrapper<const T>> the return list of statement that is matched by the predicate
 */
template<typename T>
MatchedNodes<T> extractPattern(ast::ASTNode *root, std::function<bool(const T&)> isMatch) {
    SingleCollector<T> collector;
    root->accept(&collector);
    MatchedNodes<T> matched;
    for (auto stmt : collector.nodes) {
        if (isMatch(stmt)) {
            matched.emplace_back(stmt);
        }
    }
    return matched;
}

/**
 * @brief Make a predicate for pattern matching conditional statements
 * 
 * @tparam T the type of conditional statement. In this case ast::If / ast::While
 * @param var the constraint of the var
 * @return std::function<bool(const T&)> a predicate function to check a given conditional statement of type T
 */
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

/**
 * @brief Make a predicate for pattern matching assign statements
 * 
 * @param lhs the constraint on the lhs of the statement, which should be a var
 * @param rhs the constraint on the rhs of the statement, which should be an expression
 * @param isStrict boolean flag to indicate if strict matching is used on expression.
 * @return std::function<bool(const ast::Assign&)> a predicate function to check a given assign statement
 */
std::function<bool(const ast::Assign&)> makeAssignPredicate(PatternParam lhs, PatternParam rhs) {
    return [lhs, rhs](const ast::Assign& node) {
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
        if (rhs.isStrict) {
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

MatchedNodes<ast::If> extractIf(ast::ASTNode *root, PatternParam var) {
    return extractPattern<ast::If>(root, makeCondPredicate<ast::If>(var));
}

MatchedNodes<ast::While> extractWhile(ast::ASTNode *root, PatternParam var) {
    return extractPattern<ast::While>(root, makeCondPredicate<ast::While>(var));
}

MatchedNodes<ast::Assign> extractAssign(ast::ASTNode *root, PatternParam lhs, PatternParam rhs) {
    return extractPattern<ast::Assign>(root, makeAssignPredicate(lhs, rhs));
};

}  // namespace design_extractor
}  // namespace sp
