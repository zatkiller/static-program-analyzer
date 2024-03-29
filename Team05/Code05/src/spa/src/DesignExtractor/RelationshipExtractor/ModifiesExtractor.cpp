#include "logging.h"
#include "ModifiesExtractor.h"

#define DEBUG_LOG Logger(Level::DEBUG) << "ModifiesExtractor.cpp Extracted "
namespace sp {
namespace design_extractor {
/**
 * a ASTNodeVisitor that collects the Modifies relationship as it walks through the AST.
 */
class ModifiesCollector : public TransitiveRelationshipTemplate {
private:
    void insert(Content, Content);
public:
    using TransitiveRelationshipTemplate::TransitiveRelationshipTemplate;
    void visit(const ast::Read&) override;
    void visit(const ast::Assign&) override;
    void visit(const ast::While&) override;
    void visit(const ast::If&) override;
};

std::set<Entry> ModifiesExtractor::extract(const ast::ASTNode* node) {
    ModifiesCollector collector;
    collector.extract(node);
    return collector.getEntries();
}

void ModifiesCollector::visit(const ast::Read& node) {
    extractAndInsert(STMT_LO{node.getStmtNo(), StatementType::Read}, &node);
}

void ModifiesCollector::visit(const ast::Assign& node) {
    extractAndInsert(STMT_LO{node.getStmtNo(), StatementType::Assignment}, node.getLHS());
}

void ModifiesCollector::visit(const ast::While& node) {
    stmtNumToType[node.getStmtNo()] = StatementType::While;
}

void ModifiesCollector::visit(const ast::If& node) {
    stmtNumToType[node.getStmtNo()] = StatementType::If;
}

void ModifiesCollector::insert(Content a1, Content a2) {
    auto relationship = Relationship(PKBRelationship::MODIFIES, a1, a2);
    entries.insert(relationship);
}
}  // namespace design_extractor
}  // namespace sp
