#include "VariableExtractor.h"
#include "logging.h"

namespace sp {
void VariableExtractor::visit(const sp::ast::Var& node) {
    Logger(Level::DEBUG) << "VariableExtractor.cpp Extracted variable " << node.getVarName();
    pkb->insertVariable(node.getVarName());
}
}  // namespace sp
