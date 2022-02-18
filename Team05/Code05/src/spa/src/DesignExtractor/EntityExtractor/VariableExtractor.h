#pragma once

#include <string>

#include "DesignExtractor/Extractor.h"

namespace sp {
/**
 * Extracts all variables from the AST and send them to PKB Adaptor.
 */
class VariableExtractor : public Extractor {
public:
    using Extractor::Extractor;
    void visit(const sp::ast::Var& node) override;
};
}  // sp
