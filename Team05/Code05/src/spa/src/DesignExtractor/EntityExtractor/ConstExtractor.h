#pragma once

#include "DesignExtractor/Extractor.h"


namespace sp {
namespace design_extractor {
/**
 * Extracts all const from the AST and send them to PKB Adaptor.
 */
class ConstExtractor : public Extractor {
public:
    using Extractor::Extractor;
    void visit(const ast::Const& node) override;
};

}  // namespace design_extractor
}  // namespace sp