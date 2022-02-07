#include "StatementExtractor.h"
#include "logging.h"

#define DEBUG_LOG Logger(Level::DEBUG) << "StatementExtractor.cpp Extracted "

void StatementExtractor::visit(const AST::Read& node) {
	DEBUG_LOG << "Read Stmt " << node.getStmtNo();
	pkb->insertStatement(StatementType::Read, node.getStmtNo());
}

void StatementExtractor::visit(const AST::Print& node) {
	DEBUG_LOG << "Print Stmt " << node.getStmtNo();
	pkb->insertStatement(StatementType::Print, node.getStmtNo());
}

void StatementExtractor::visit(const AST::While& node) {
	DEBUG_LOG << "While Stmt " << node.getStmtNo();
	pkb->insertStatement(StatementType::While, node.getStmtNo());
}

void StatementExtractor::visit(const AST::If& node) {
	DEBUG_LOG << "If Stmt " << node.getStmtNo();
	pkb->insertStatement(StatementType::If, node.getStmtNo());
}

void StatementExtractor::visit(const AST::Assign& node) {
	DEBUG_LOG << "Assign Stmt " << node.getStmtNo();
	pkb->insertStatement(StatementType::Assignment, node.getStmtNo());
}
