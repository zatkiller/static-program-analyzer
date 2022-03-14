#include "VariableExtractor.h"
#include "logging.h"

namespace sp {
namespace design_extractor {
// void VariableExtractor::visit(const ast::Var& node) {
//     Logger(Level::DEBUG) << "VariableExtractor.cpp Extracted variable " << node.getVarName();
//     pkb->insertEntity(VAR_NAME{node.getVarName()});
// }

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
