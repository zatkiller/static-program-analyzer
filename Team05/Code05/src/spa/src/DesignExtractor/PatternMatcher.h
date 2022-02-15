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
using AssignPatternReturn = std::list<std::reference_wrapper<const AST::Assign>>;

AssignPatternReturn extractAssign(AST::ASTNode *root, PatternParam lhs, PatternParam rhs);
