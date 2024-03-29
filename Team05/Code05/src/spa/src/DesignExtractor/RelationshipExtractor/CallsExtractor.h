#pragma once

#include "DesignExtractor/Extractor.h"

namespace sp {
namespace design_extractor {
/**
 * Extracts all Calls relationship from the AST and return them as a set of entries
 */
struct CallsExtractor : public Extractor<const ast::ASTNode*> {
    std::set<Entry> extract(const ast::ASTNode*) override;
};

/**
 * Extracts all Calls relationship from the AST and send them to the PKB
 */
struct CallsExtractorModule : public ExtractorModule<const ast::ASTNode*> {
public:
    explicit CallsExtractorModule(PKB *pkb) : 
        ExtractorModule(std::make_unique<CallsExtractor>(), pkb) {}
};

}  // namespace design_extractor
}  // namespace sp
