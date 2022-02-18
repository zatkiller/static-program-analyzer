#include "StatementExtractor.h"
#include "logging.h"

#define DEBUG_LOG Logger(Level::DEBUG) << "StatementExtractor.cpp Extracted "

namespace sp {
void StatementExtractor::visit(const sp::ast::Read& node) {
    DEBUG_LOG << "Read Stmt " << node.getStmtNo();
    pkb->insertStatement(STMT_LO{node.getStmtNo(), StatementType::Read});
}

void StatementExtractor::visit(const sp::ast::Print& node) {
    DEBUG_LOG << "Print Stmt " << node.getStmtNo();
    pkb->insertStatement(STMT_LO{node.getStmtNo(), StatementType::Print});
}

void StatementExtractor::visit(const sp::ast::While& node) {
    DEBUG_LOG << "While Stmt " << node.getStmtNo();
    pkb->insertStatement(STMT_LO{node.getStmtNo(), StatementType::While});
}

void StatementExtractor::visit(const sp::ast::If& node) {
    DEBUG_LOG << "If Stmt " << node.getStmtNo();
    pkb->insertStatement(STMT_LO{node.getStmtNo(), StatementType::If});
}

void StatementExtractor::visit(const sp::ast::Assign& node) {
    DEBUG_LOG << "Assign Stmt " << node.getStmtNo();
    pkb->insertStatement(STMT_LO{node.getStmtNo(), StatementType::Assignment});
}
}  // namespace sp
