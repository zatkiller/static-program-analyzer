#include "UsesExtractor.h"
#include "logging.h"

#define DEBUG_LOG Logger(Level::DEBUG) << "UsesExtractor.cpp Extracted "

namespace sp {
void UsesExtractor::visit(const sp::ast::Print& node) {
    extractAndInsert(STMT_LO{node.getStmtNo(), StatementType::Print}, &node);
}

void UsesExtractor::visit(const sp::ast::Assign& node) {
    extractAndInsert(STMT_LO{node.getStmtNo(), StatementType::Assignment}, node.getRHS());
}

void UsesExtractor::visit(const sp::ast::While& node) {
    stmtNumToType[node.getStmtNo()] = StatementType::While;
    extractAndInsert(STMT_LO{node.getStmtNo(), StatementType::While}, node.getCondExpr());
}
void UsesExtractor::visit(const sp::ast::If& node) {
    stmtNumToType[node.getStmtNo()] = StatementType::If;
    extractAndInsert(STMT_LO{node.getStmtNo(), StatementType::If}, node.getCondExpr());
}

void UsesExtractor::insert(Content a1, Content a2) {
    pkb->insertRelationship(PKBRelationship::USES, a1, a2);
}
}
