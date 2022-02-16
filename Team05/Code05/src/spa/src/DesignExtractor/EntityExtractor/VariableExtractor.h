#pragma once

#include <string>

#include "DesignExtractor/Extractor.h"


/**
 * Extracts all variables from the AST and send them to PKB Adaptor.
 */
class VariableExtractor : public Extractor {
public:
    using Extractor::Extractor;
    void visit(const AST::Var& node) override;
};
