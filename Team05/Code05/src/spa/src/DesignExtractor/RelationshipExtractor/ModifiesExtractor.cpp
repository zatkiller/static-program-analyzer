#include "ModifiesExtractor.h"
#include "logging.h"


#define DEBUG_LOG Logger(Level::DEBUG) << "ModifiesExtractor.cpp Extracted "
namespace sp {
namespace design_extractor {
void ModifiesExtractor::visit(const ast::Read& node) {
    extractAndInsert(STMT_LO{node.getStmtNo(), StatementType::Read}, &node);
}

void ModifiesExtractor::visit(const ast::Assign& node) {
    extractAndInsert(STMT_LO{node.getStmtNo(), StatementType::Assignment}, node.getLHS());
}

void ModifiesExtractor::visit(const ast::While& node) {
    stmtNumToType[node.getStmtNo()] = StatementType::While;
}

void ModifiesExtractor::visit(const ast::If& node) {
    stmtNumToType[node.getStmtNo()] = StatementType::If;
}

void ModifiesExtractor::insert(Content a1, Content a2) {
    pkb->insertRelationship(PKBRelationship::MODIFIES, a1, a2);
}
}  // namespace design_extractor
}  // namespace sp
