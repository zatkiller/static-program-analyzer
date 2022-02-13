#pragma once

#include <deque>
#include <string>
#include <map>

#include "Parser/AST.h"
#include "DesignExtractor/RelationshipExtractor/TransitiveRelationshipTemplate.h"
#include "PKB.h"


/**
 * Extracts all uses relationship from the AST and send them to PKB Adaptor.
 */
class ModifiesExtractor : public TransitiveRelationshipTemplate {
private:
    void insert(Content a1, Content a2);
public:
    using TransitiveRelationshipTemplate::TransitiveRelationshipTemplate;
    void visit(const AST::Read& node) override;
    void visit(const AST::Assign& node) override;
    void visit(const AST::While&) override;
    void visit(const AST::If&) override;
};
