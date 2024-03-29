#pragma once

#include <deque>
#include <string>
#include <map>
#include <set>

#include "DesignExtractor/Extractor.h"
#include "DesignExtractor/RelationshipExtractor/TransitiveRelationshipTemplate.h"
#include "Parser/AST.h"
#include "PKB.h"

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
 * Extracts all Uses relationship from the AST and send them to the PKB
 */
class UsesExtractorModule : public ExtractorModule<const ast::ASTNode*> {
public:
    explicit UsesExtractorModule(PKB *pkb) : 
        ExtractorModule(std::make_unique<UsesExtractor>(), pkb) {}
};

}  // namespace design_extractor
}  // namespace sp
