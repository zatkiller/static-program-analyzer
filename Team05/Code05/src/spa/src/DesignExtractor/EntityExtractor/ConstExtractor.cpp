#include "ConstExtractor.h"
#include "PKB/PKBDataTypes.h"

void ConstExtractor::visit(const AST::Const& node) {
    Logger(Level::DEBUG) << "ConstExtractor.cpp Extracted const " << node.getConstValue();
    pkb->insertEntity(CONST{node.getConstValue()});
};
