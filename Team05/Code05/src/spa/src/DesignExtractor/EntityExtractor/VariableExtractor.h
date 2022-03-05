#pragma once

#include <string>

#include "DesignExtractor/Extractor.h"

namespace sp {
namespace design_extractor {
/**
 * Extracts all variables from the AST and send them to PKB Adaptor.
 */
class VariableExtractor : public Extractor {
public:
    using Extractor::Extractor;
    void visit(const ast::Var& node) override;
};
}  // namepsace design_extractor
}  // namespace sp
