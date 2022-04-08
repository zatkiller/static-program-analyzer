#pragma once

#include <list>
#include <optional>

#include "Parser/AST.h"

namespace sp {
namespace design_extractor {

template <typename T>
struct MatchedNodes : public std::list<std::reference_wrapper<const T>>{};

/**
 * @brief Encapsualtes a pattern matching parameter
 * 
 * Using a combination of boolean and optional to encapsulate the different types of parameter for pattern matching.
 * 
 * Examples:
 * 
 * For "value" parameter, it should be PatternParam("value", true)
 * For _"value"_ parameter, it should be PatternParam("value", false)
 * For wildcard _ paramater, it should be PatternParam(std::nullopt)
 */
struct PatternParam : public std::optional<std::string> {
    bool isStrict;
    explicit PatternParam(std::optional<std::string> value) : std::optional<std::string>(value), isStrict(true) {}
    PatternParam(std::optional<std::string> value, bool isStrict) :
        std::optional<std::string>(value), isStrict(isStrict) {}
};

/**
 * @brief Extracts all assignment statements in AST with given root that satisfy the constraint given.
 * 
 * @param root The root of the AST
 * @param lhs The optional string constraint of LHS variable being assigned. Use std::nullopt if LHS is wildcard or synonym. 
 * @param rhs The optional string constraint of RHS expression being used. Use std::nullopt if RHS  is wildcard.
 * @param isStrict defaults to false. If it is true, then rhs needs to be a strict match to the pattern.
 * @return AssignPatternReturn return a list of references to the assign nodes.
 */
MatchedNodes<ast::Assign> extractAssign(ast::ASTNode *root, PatternParam lhs, PatternParam rhs);
MatchedNodes<ast::If> extractIf(ast::ASTNode *root, PatternParam var);
MatchedNodes<ast::While> extractWhile(ast::ASTNode *root, PatternParam var);


}  // namespace design_extractor
}  // namespace sp
