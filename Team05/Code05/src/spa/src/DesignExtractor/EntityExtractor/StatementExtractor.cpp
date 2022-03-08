#include "StatementExtractor.h"
#include "logging.h"

#define DEBUG_LOG Logger(Level::DEBUG) << "StatementExtractor.cpp Extracted "

namespace sp {
namespace design_extractor {
void StatementExtractor::visit(const ast::Read& node) {
    DEBUG_LOG << "Read Stmt " << node.getStmtNo();
    pkb->insertEntity(STMT_LO{node.getStmtNo(), StatementType::Read});
}

void StatementExtractor::visit(const ast::Print& node) {
    DEBUG_LOG << "Print Stmt " << node.getStmtNo();
    pkb->insertEntity(STMT_LO{node.getStmtNo(), StatementType::Print});
}

void StatementExtractor::visit(const ast::While& node) {
    DEBUG_LOG << "While Stmt " << node.getStmtNo();
    pkb->insertEntity(STMT_LO{node.getStmtNo(), StatementType::While});
}

void StatementExtractor::visit(const ast::If& node) {
    DEBUG_LOG << "If Stmt " << node.getStmtNo();
    pkb->insertEntity(STMT_LO{node.getStmtNo(), StatementType::If});
}

void StatementExtractor::visit(const ast::Assign& node) {
    DEBUG_LOG << "Assign Stmt " << node.getStmtNo();
    pkb->insertEntity(STMT_LO{node.getStmtNo(), StatementType::Assignment});
}

void StatementExtractor::visit(const ast::Call& node) {
    DEBUG_LOG << "Call Stmt " << node.getStmtNo();
    pkb->insertEntity(STMT_LO(node.getStmtNo(), StatementType::Call));
}
}  // namespace design_extractor
}  // namespace sp
