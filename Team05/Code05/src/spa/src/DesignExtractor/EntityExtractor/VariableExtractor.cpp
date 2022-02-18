#include "VariableExtractor.h"
#include "logging.h"

namespace sp {
namespace design_extractor {
void VariableExtractor::visit(const ast::Var& node) {
    Logger(Level::DEBUG) << "VariableExtractor.cpp Extracted variable " << node.getVarName();
    pkb->insertVariable(node.getVarName());
}
}  // namespace design_extractor
}  // namespace sp
