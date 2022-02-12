#include "UsesExtractor.h"
#include "logging.h"

#define DEBUG_LOG Logger(Level::DEBUG) << "UsesExtractor.cpp Extracted "

void UsesExtractor::visit(const AST::Print& node) {
    extractAndInsert(STMT_LO{node.getStmtNo(), StatementType::Print}, &node);
};
void UsesExtractor::visit(const AST::Assign& node) {
    extractAndInsert(STMT_LO{node.getStmtNo(), StatementType::Assignment}, node.getRHS());
};

void UsesExtractor::visit(const AST::While& node) {
    stmtNumToType[node.getStmtNo()] = StatementType::While;
    extractAndInsert(STMT_LO{node.getStmtNo(), StatementType::While}, node.getCondExpr());
};
void UsesExtractor::visit(const AST::If& node) {
    stmtNumToType[node.getStmtNo()] = StatementType::If;
    extractAndInsert(STMT_LO{node.getStmtNo(), StatementType::If}, node.getCondExpr());
};

void UsesExtractor::insert(Content a1, Content a2) {
    pkb->insertRelationship(PKBRelationship::USES, a1, a2);
};

