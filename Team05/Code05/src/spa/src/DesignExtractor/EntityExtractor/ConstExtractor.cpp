#include "ConstExtractor.h"
#include "PKB/PKBField.h"

namespace sp {
namespace design_extractor {
struct ConstWalker : public TreeWalker {
    std::set<Entry> procs;
    void visit(const ast::Const& node) {
        procs.insert(CONST{node.getConstValue()});
    }
};

std::set<Entry> ConstExtractor::extract(const ast::ASTNode* node) {
    ConstWalker extractor;
    node->accept(&extractor);
    return extractor.procs;
};

}  // namespace design_extractor
}  // namespace sp
