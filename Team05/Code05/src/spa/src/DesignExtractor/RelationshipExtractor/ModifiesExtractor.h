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
 * Extracts all uses relationship from the AST and send them to PKB Adaptor.
 */
class ModifiesExtractor : public TransitiveRelationshipTemplate {
private:
    void insert(Content a1, Content a2);
public:
    using TransitiveRelationshipTemplate::TransitiveRelationshipTemplate;
    void visit(const ast::Read& node) override;
    void visit(const ast::Assign& node) override;
    void visit(const ast::While&) override;
    void visit(const ast::If&) override;
};
}  // namespace design_extractor
}  // namespace sp
