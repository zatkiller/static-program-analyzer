#pragma once

#include <deque>
#include <string>
#include <map>
#include <set>

#include "Parser/AST.h"
#include "DesignExtractor/Extractor.h"
#include "PKB.h"
#include "DesignExtractor/RelationshipExtractor/TransitiveRelationshipTemplate.h"

namespace sp {
namespace design_extractor {
/**
 * Extracts all uses relationship from the AST and send them to PKB Adaptor.
 */
class UsesExtractor : public IExtractor {
public:
    using IExtractor::IExtractor;
    std::set<Entry> extract(const ast::ASTNode*) override;
};

class UsesExtractorModule : public ExtractorModule {
public:
    UsesExtractorModule(PKBStrategy *pkb) : 
        ExtractorModule(std::make_unique<UsesExtractor>(), pkb) {};
};

}  // namespace design_extractor
}  // namespace sp
