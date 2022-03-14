#pragma once

#include <string>

#include "DesignExtractor/Extractor.h"

namespace sp {
namespace design_extractor {
/**
 * Extracts all variables from the AST and send them to PKB Adaptor.
 */
class VariableExtractor : public IExtractor {
public:
    using IExtractor::IExtractor;
    std::set<Entry> extract(const ast::ASTNode*) override;
};


class VariableExtractorModule : public ExtractorModule {
public:
    VariableExtractorModule(PKBStrategy *pkb) : 
        ExtractorModule(std::make_unique<VariableExtractor>(), pkb) {};
};

}  // namepsace design_extractor
}  // namespace sp
