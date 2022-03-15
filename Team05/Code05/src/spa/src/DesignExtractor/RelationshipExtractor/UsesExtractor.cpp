#include "UsesExtractor.h"
#include "logging.h"

#define DEBUG_LOG Logger(Level::DEBUG) << "UsesExtractor.cpp Extracted "

namespace sp {
namespace design_extractor {


/**
 * Extracts all uses relationship from the AST and send them to PKB Adaptor.
 */
class UsesWalker : public TransitiveRelationshipTemplate {
private:
    void insert(Content a1, Content a2);
public:
    using TransitiveRelationshipTemplate::TransitiveRelationshipTemplate;
    void visit(const ast::Print& node) override;
    void visit(const ast::Assign& node) override;
    void visit(const ast::While&) override;
    void visit(const ast::If&) override;
};

std::set<Entry> UsesExtractor::extract(const ast::ASTNode* node) {
    UsesWalker extractor;
    extractor.extract(node);
    return extractor.relationships;
};

void UsesWalker::visit(const ast::Print& node) {
    extractAndInsert(STMT_LO{node.getStmtNo(), StatementType::Print}, &node);
}

void UsesWalker::visit(const ast::Assign& node) {
    extractAndInsert(STMT_LO{node.getStmtNo(), StatementType::Assignment}, node.getRHS());
}

void UsesWalker::visit(const ast::While& node) {
    stmtNumToType[node.getStmtNo()] = StatementType::While;
    extractAndInsert(STMT_LO{node.getStmtNo(), StatementType::While}, node.getCondExpr());
}
void UsesWalker::visit(const ast::If& node) {
    stmtNumToType[node.getStmtNo()] = StatementType::If;
    extractAndInsert(STMT_LO{node.getStmtNo(), StatementType::If}, node.getCondExpr());
}

void UsesWalker::insert(Content a1, Content a2) {
    auto relationship = Relationship(PKBRelationship::USES, a1, a2);
    relationships.insert(relationship);
}
}  // namespace design_extractor
}  // namespace sp
