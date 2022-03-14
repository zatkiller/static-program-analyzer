#include "StatementExtractor.h"
#include "logging.h"

#define DEBUG_LOG Logger(Level::DEBUG) << "StatementExtractor.cpp Extracted "

namespace sp {
namespace design_extractor {

struct StatementWalker : public TreeWalker {
    std::set<Entry> statements;
    void visit(const ast::Read&) override;
    void visit(const ast::Print&) override;
    void visit(const ast::While&) override;
    void visit(const ast::If&) override;
    void visit(const ast::Assign&) override;
    void visit(const ast::Call&) override;
};

std::set<Entry> StatementExtractor::extract(const ast::ASTNode* node) {
    StatementWalker extractor;
    node->accept(&extractor);
    return extractor.statements;
}

void StatementWalker::visit(const ast::Read& node) {
    DEBUG_LOG << "Read Stmt " << node.getStmtNo();
    statements.insert(STMT_LO{node.getStmtNo(), StatementType::Read});
}

void StatementWalker::visit(const ast::Print& node) {
    DEBUG_LOG << "Print Stmt " << node.getStmtNo();
    statements.insert(STMT_LO{node.getStmtNo(), StatementType::Print});
}

void StatementWalker::visit(const ast::While& node) {
    DEBUG_LOG << "While Stmt " << node.getStmtNo();
    statements.insert(STMT_LO{node.getStmtNo(), StatementType::While});
}

void StatementWalker::visit(const ast::If& node) {
    DEBUG_LOG << "If Stmt " << node.getStmtNo();
    statements.insert(STMT_LO{node.getStmtNo(), StatementType::If});
}

void StatementWalker::visit(const ast::Assign& node) {
    DEBUG_LOG << "Assign Stmt " << node.getStmtNo();
    statements.insert(STMT_LO{node.getStmtNo(), StatementType::Assignment});
}

void StatementWalker::visit(const ast::Call& node) {
    DEBUG_LOG << "Call Stmt " << node.getStmtNo();
    statements.insert(STMT_LO(node.getStmtNo(), StatementType::Call));
}
}  // namespace design_extractor
}  // namespace sp
