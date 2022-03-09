#include "CallsExtractor.h"
#include "PKB.h"

namespace sp {
namespace design_extractor {

void CallsExtractor::visit(const ast::Procedure& node) {
    currentProc = node.getName();
}

void CallsExtractor::visit(const ast::Call& node) {
    if (!currentProc.empty()) {
        pkb->insertRelationship(PKBRelationship::CALLS, PROC_NAME{currentProc}, PROC_NAME{node.getName()});
    }
}

}  // namespace design_extractor
}  // namespace sp