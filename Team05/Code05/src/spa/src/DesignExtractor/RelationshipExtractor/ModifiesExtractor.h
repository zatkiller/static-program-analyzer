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
 * Extracts all modifies relationship from the AST and send them to PKB Adaptor.
 */
class ModifiesExtractor : public IExtractor {
public:
    using IExtractor::IExtractor;
    std::set<Entry> extract(const ast::ASTNode*) override;
};

class ModifiesExtractorModule : public ExtractorModule {
public:
    ModifiesExtractorModule(PKBStrategy *pkb) : 
        ExtractorModule(std::make_unique<ModifiesExtractor>(), pkb) {};
};
}  // namespace design_extractor
}  // namespace sp
