#pragma once

#include "DesignExtractor/Extractor.h"


/**
 * Extracts all const from the AST and send them to PKB Adaptor.
 */
class ConstExtractor : public Extractor {
public:
    using Extractor::Extractor;
    void visit(const AST::Const& node) override;
};
