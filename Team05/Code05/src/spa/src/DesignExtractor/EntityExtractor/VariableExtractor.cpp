#include "VariableExtractor.h"
#include "logging.h"

namespace sp {
namespace design_extractor {

struct VariableWalker : public TreeWalker {
    std::set<Entry> vars;
    void visit(const ast::Var& node) {
        vars.insert(VAR_NAME{node.getVarName()});
    }
};

std::set<Entry> VariableExtractor::extract(const ast::ASTNode* node) {
    VariableWalker extractor;
    node->accept(&extractor);
    return extractor.vars;
};

}  // namespace design_extractor
}  // namespace sp
