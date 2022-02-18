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
class UsesExtractor : public TransitiveRelationshipTemplate {
private:
    void insert(Content a1, Content a2);
public:
    using TransitiveRelationshipTemplate::TransitiveRelationshipTemplate;
    void visit(const ast::Print& node) override;
    void visit(const ast::Assign& node) override;
    void visit(const ast::While&) override;
    void visit(const ast::If&) override;
};
}  // namespace design_extractor
}  // namespace sp
