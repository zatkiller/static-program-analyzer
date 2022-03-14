#include "ProcedureExtractor.h"
#include "PKB/PKBField.h"


namespace sp {
namespace design_extractor {
struct ProcedureWalker : public TreeWalker {
    std::set<Entry> procs;
    void visit(const ast::Procedure& node) {
        procs.insert(PROC_NAME{node.getName()});
    }
};

std::set<Entry> ProcedureExtractor::extract(const ast::ASTNode* node) {
    ProcedureWalker extractor;
    node->accept(&extractor);
    return extractor.procs;
};


}  // namespace design_extractor
}  // namespace sp
