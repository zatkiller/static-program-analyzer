#pragma once

#include "DesignExtractor/Extractor.h"

namespace sp {
namespace design_extractor {
/**
 * Extracts all Calls relationship from the AST and return them as a set of entries
 */
struct CallsExtractor : public Extractor {
    std::set<Entry> extract(const ast::ASTNode*) override;
};

/**
 * Extracts all Calls relationship from the AST and send them to the PKBStrategy
 */
struct CallsExtractorModule : public ExtractorModule {
public:
    CallsExtractorModule(PKBStrategy *pkb) : 
        ExtractorModule(std::make_unique<CallsExtractor>(), pkb) {};
};

}  // namespace design_extractor
}  // namespace sp
