#pragma once

#include <list>

#include "PKB.h"
#include "Parser/AST.h"
#include "DesignExtractor/Extractor.h"
#include "DesignExtractor/EntityExtractor/EntityExtractor.h"
#include "DesignExtractor/RelationshipExtractor/ModifiesExtractor.h"
#include "DesignExtractor/RelationshipExtractor/UsesExtractor.h"
#include "DesignExtractor/RelationshipExtractor/FollowsExtractor.h"
#include "DesignExtractor/RelationshipExtractor/ParentExtractor.h"
#include "DesignExtractor/RelationshipExtractor/CallsExtractor.h"

namespace sp {
namespace design_extractor {
class DesignExtractor {
private:
    std::list<std::shared_ptr<Extractor>> extractors;
    PKBStrategy* pkbStrategy;
public:
    explicit DesignExtractor(PKBStrategy* pkbStrategy) : pkbStrategy(pkbStrategy) {
        // extractors.push_back(std::make_shared<StatementExtractorModule>(pkbStrategy));
        // extractors.push_back(std::make_shared<VariableExtractorModule>(pkbStrategy));
        // extractors.push_back(std::make_shared<ConstExtractor>(pkbStrategy));
        // extractors.push_back(std::make_shared<ProcedureExtractor>(pkbStrategy));
        // extractors.push_back(std::make_shared<ModifiesExtractor>(pkbStrategy));
        // extractors.push_back(std::make_shared<UsesExtractorModule>(pkbStrategy));
        // extractors.push_back(std::make_shared<FollowsExtractorModule>(pkbStrategy));
        // extractors.push_back(std::make_shared<ParentExtractorModule>(pkbStrategy));
        // extractors.push_back(std::make_shared<CallsExtractorModule>(pkbStrategy));
    }

    void extract(ast::ASTNode* ast) {
        for (auto extractor : extractors) {
            extractor->extract(ast);
        }
    }
};
}  // namespace design_extractor
}  // namespace sp
