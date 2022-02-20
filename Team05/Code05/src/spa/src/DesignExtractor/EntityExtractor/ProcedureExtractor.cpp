#include "ProcedureExtractor.h"
#include "PKB/PKBDataTypes.h"


namespace sp {
namespace design_extractor {

void ProcedureExtractor::visit(const ast::Procedure& node) {
    Logger(Level::DEBUG) << "ProcedureExtractor.cpp Extracted const " << node.getName();
    pkb->insertEntity(PROC_NAME{node.getName()});
};

}  // namespace design_extractor
}  // namespace sp
