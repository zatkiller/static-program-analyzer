#pragma once

#include <deque>
#include <string>
#include <map>

#include "Parser/AST.h"
#include "DesignExtractor/RelationshipExtractor/TransitiveRelationshipTemplate.h"
#include "PKB.h"

namespace sp {
namespace design_extractor {
/**
 * Extracts all Modifies relationship from the AST and return them as a set of entries
 */
class ModifiesExtractor : public Extractor {
public:
    using Extractor::Extractor;
    std::set<Entry> extract(const ast::ASTNode*) override;
};

/**
 * Extracts all Modifies relationship from the AST and send them to the PKBStrategy
 */
class ModifiesExtractorModule : public ExtractorModule {
public:
    ModifiesExtractorModule(PKBStrategy *pkb) : 
        ExtractorModule(std::make_unique<ModifiesExtractor>(), pkb) {};
};
}  // namespace design_extractor
}  // namespace sp
