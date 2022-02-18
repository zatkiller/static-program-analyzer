#pragma once

#include <list>
#include <optional>
#include <algorithm>

#include "DesignExtractor.h"
#include "Parser/AST.h"
#include "Parser/Lexer.h"
#include "Parser/Parser.h"
#include "PKB/PKBDataTypes.h"

namespace sp {
using PatternParam = std::optional<std::string>;
using AssignPatternReturn = std::list<std::reference_wrapper<const sp::ast::Assign>>;

/**
 * @brief Extracts all assignment statements in AST with given root that satisfy the constraint given.
 * 
 * Currently it does not support strict matching like pattern a(v, "x")
 * Only partial matching like pattern a(v, _"x+1"_) is supported.
 * 
 * @param root The root of the AST
 * @param lhs The optional string constraint of LHS variable being assigned. Use std::nullopt if LHS is wildcard or synonym. 
 * @param rhs The optional string constraint of RHS expression being used. Use std::nullopt if RHS  is wildcard.
 * @return AssignPatternReturn return a list of references to the assign nodes.
 */
AssignPatternReturn extractAssign(sp::ast::ASTNode *root, PatternParam lhs, PatternParam rhs);
}  // namespace sp
