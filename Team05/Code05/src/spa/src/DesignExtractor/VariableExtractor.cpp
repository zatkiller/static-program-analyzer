#include "VariableExtractor.h"
#include "logging.h"

void VariableExtractor::visit(const AST::Var& node) {
    Logger(Level::DEBUG) << "VariableExtractor.cpp Extracted variable " << node.getVarName();

    // local table for testing
    table.insert(node.getVarName());

    pkb->insertVariable(node.getVarName());
}
