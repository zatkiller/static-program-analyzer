#include "DesignExtractor.h"

void VariableExtractor::visit(const AST::Var& node) {
    pkb->insert("variables", node.getVarName());
}
