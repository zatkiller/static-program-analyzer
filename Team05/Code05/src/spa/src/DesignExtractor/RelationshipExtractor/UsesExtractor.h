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
 * Extracts all Uses relationship from the AST and return them as a set of entries
 */
class UsesExtractor : public Extractor<const ast::ASTNode*> {
public:
    using Extractor::Extractor;
    std::set<Entry> extract(const ast::ASTNode*) override;
};

/**
 * Extracts all Uses relationship from the AST and send them to the PKBStrategy
 */
class UsesExtractorModule : public ExtractorModule<const ast::ASTNode*> {
public:
    explicit UsesExtractorModule(PKBStrategy *pkb) : 
        ExtractorModule(std::make_unique<UsesExtractor>(), pkb) {}
};

}  // namespace design_extractor
}  // namespace sp
