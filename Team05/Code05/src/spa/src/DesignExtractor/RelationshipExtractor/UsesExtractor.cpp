#include "UsesExtractor.h"
#include "logging.h"

#define DEBUG_LOG Logger(Level::DEBUG) << "UsesExtractor.cpp Extracted "

namespace sp {
namespace design_extractor {
void UsesExtractor::visit(const ast::Print& node) {
    extractAndInsert(STMT_LO{node.getStmtNo(), StatementType::Print}, &node);
}

void UsesExtractor::visit(const ast::Assign& node) {
    extractAndInsert(STMT_LO{node.getStmtNo(), StatementType::Assignment}, node.getRHS());
}

void UsesExtractor::visit(const ast::While& node) {
    stmtNumToType[node.getStmtNo()] = StatementType::While;
    extractAndInsert(STMT_LO{node.getStmtNo(), StatementType::While}, node.getCondExpr());
}
void UsesExtractor::visit(const ast::If& node) {
    stmtNumToType[node.getStmtNo()] = StatementType::If;
    extractAndInsert(STMT_LO{node.getStmtNo(), StatementType::If}, node.getCondExpr());
}

void UsesExtractor::insert(Content a1, Content a2) {
    pkb->insertRelationship(PKBRelationship::USES, a1, a2);
}
}  // namespace design_extractor
}  // namespace sp
