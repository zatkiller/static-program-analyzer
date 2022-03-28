#pragma once

#include <list>
#include <optional>

#include "Parser/AST.h"

namespace sp {
namespace design_extractor {
using PatternParam = std::optional<std::string>;
using AssignPatternReturn = std::list<std::reference_wrapper<const ast::Assign>>;
using IfPatternReturn = std::list<std::reference_wrapper<const ast::If>>;
using WhilePatternReturn = std::list<std::reference_wrapper<const ast::While>>;
/**
 * @brief Extracts all assignment statements in AST with given root that satisfy the constraint given.
 * 
 * @param root The root of the AST
 * @param lhs The optional string constraint of LHS variable being assigned. Use std::nullopt if LHS is wildcard or synonym. 
 * @param rhs The optional string constraint of RHS expression being used. Use std::nullopt if RHS  is wildcard.
 * @param isStrict defaults to false. If it is true, then rhs needs to be a strict match to the pattern.
 * @return AssignPatternReturn return a list of references to the assign nodes.
 */
AssignPatternReturn extractAssign(ast::ASTNode *root, PatternParam lhs, PatternParam rhs, bool isStrict = false);
IfPatternReturn extractIf(ast::ASTNode *root, PatternParam var);
WhilePatternReturn extractWhile(ast::ASTNode *root, PatternParam var);


}  // namespace design_extractor
}  // namespace sp
