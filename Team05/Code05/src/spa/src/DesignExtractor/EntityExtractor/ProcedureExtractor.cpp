#include "ProcedureExtractor.h"
#include "PKB/PKBDataTypes.h"

void ProcedureExtractor::visit(const AST::Procedure& node) {
    Logger(Level::DEBUG) << "ProcedureExtractor.cpp Extracted const " << node.getName();
    pkb->insertEntity(PROC_NAME{node.getName()});
};
