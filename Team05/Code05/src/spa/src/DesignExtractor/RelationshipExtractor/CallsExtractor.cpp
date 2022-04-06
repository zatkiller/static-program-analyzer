#include "CallsExtractor.h"
#include "PKB.h"

namespace sp {
namespace design_extractor {

/**
 * a ASTNodeVisitor that collects the Calls relationship as it walks through the AST.
 */
class CallsCollector : public Collector {
private:
    std::string currentProc = "";
public:
    void visit(const ast::Procedure&) override;
    void visit(const ast::Call&) override;
};

void CallsCollector::visit(const ast::Procedure& node) {
    currentProc = node.getName();
}

void CallsCollector::visit(const ast::Call& node) {
    if (!currentProc.empty()) {
        entries.insert(Relationship(PKBRelationship::CALLS, PROC_NAME{currentProc}, PROC_NAME{node.getName()}));
    }
}

std::set<Entry> CallsExtractor::extract(const ast::ASTNode* node) {
    CallsCollector cc;
    node->accept(&cc);
    return cc.getEntries();
}

}  // namespace design_extractor
}  // namespace sp
