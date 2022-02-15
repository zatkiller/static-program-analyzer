#pragma once

#include <list>
#include <optional>
#include <algorithm>

#include "DesignExtractor.h"
#include "Parser/AST.h"
#include "Parser/Lexer.h"
#include "Parser/Parser.h"
#include "PKB/PKBDataTypes.h"

using PatternParam = std::optional<std::string>;

struct ExprFlattener : public TreeWalker {
    std::list<std::reference_wrapper<const AST::ASTNode>> nodes;
    void visit(const AST::Var& node) override {
        nodes.emplace_back(node);
    };
    void visit(const AST::Const& node) override {
        nodes.emplace_back(node);
    };
    void visit(const AST::BinExpr& node) override {
        nodes.emplace_back(node);
    };
    void visit(const AST::RelExpr& node) override {
        nodes.emplace_back(node);
    };
    void visit(const AST::CondBinExpr& node) override {
        nodes.emplace_back(node);
    };
    void visit(const AST::NotCondExpr& node) override {
        nodes.emplace_back(node);
    };

    void clear() {
        nodes.clear();
    }
};

std::list<std::reference_wrapper<const AST::ASTNode>> flatten(AST::Expr *root) {
    auto flattener = std::make_shared<ExprFlattener>();
    root->accept(flattener);
    return flattener->nodes;
};

bool isSublist(std::list<std::reference_wrapper<const AST::ASTNode>> haystack, std::list<std::reference_wrapper<const AST::ASTNode>> needle) {
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
    std::list<std::reference_wrapper<const AST::Assign>> nodes;
    bool isMatch(const AST::Assign& node) {
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
        auto tokens = SimpleParser::Lexer(rhs.value()).getTokens();
        auto expr = SimpleParser::ExprParser::parse(tokens);

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
    AssignmentPatternExtractor(PatternParam lhs, PatternParam rhs, bool isStrict=false) : lhs(lhs), rhs(rhs), isStrict(isStrict) {};

    void visit(const AST::Assign& node) override {
        if (isMatch(node)) {
            nodes.emplace_back(node);
        }
    };
};