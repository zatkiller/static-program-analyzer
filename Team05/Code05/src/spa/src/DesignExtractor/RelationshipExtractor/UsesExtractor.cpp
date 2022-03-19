#include "UsesExtractor.h"
#include "logging.h"

#define DEBUG_LOG Logger(Level::DEBUG) << "UsesExtractor.cpp Extracted "

namespace sp {
namespace design_extractor {


/**
 * a ASTNodeVisitor that collects the Follows relationship as it walks through the AST.
 */
class UsesCollector : public TransitiveRelationshipTemplate {
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
    UsesCollector extractor;
    extractor.extract(node);
    return extractor.relationships;
}

void UsesCollector::visit(const ast::Print& node) {
    extractAndInsert(STMT_LO{node.getStmtNo(), StatementType::Print}, &node);
}

void UsesCollector::visit(const ast::Assign& node) {
    extractAndInsert(STMT_LO{node.getStmtNo(), StatementType::Assignment}, node.getRHS());
}

void UsesCollector::visit(const ast::While& node) {
    stmtNumToType[node.getStmtNo()] = StatementType::While;
    extractAndInsert(STMT_LO{node.getStmtNo(), StatementType::While}, node.getCondExpr());
}
void UsesCollector::visit(const ast::If& node) {
    stmtNumToType[node.getStmtNo()] = StatementType::If;
    extractAndInsert(STMT_LO{node.getStmtNo(), StatementType::If}, node.getCondExpr());
}

void UsesCollector::insert(Content a1, Content a2) {
    auto relationship = Relationship(PKBRelationship::USES, a1, a2);
    relationships.insert(relationship);
}
}  // namespace design_extractor
}  // namespace sp
