#include "ConstExtractor.h"
#include "PKB/PKBDataTypes.h"

namespace sp {
namespace design_extractor {
void ConstExtractor::visit(const ast::Const& node) {
    Logger(Level::DEBUG) << "ConstExtractor.cpp Extracted const " << node.getConstValue();
    pkb->insertEntity(CONST{node.getConstValue()});
};

}  // namespace design_extractor
}  // namespace sp
