#pragma once

#include <list>

#include "PKB.h"
#include "Parser/AST.h"
#include "DesignExtractor/Extractor.h"
#include "DesignExtractor/EntityExtractor/EntityExtractor.h"
#include "DesignExtractor/RelationshipExtractor/RelationshipExtractor.h"

namespace sp {
namespace design_extractor {
class DesignExtractor {
private:
    std::list<std::shared_ptr<ExtractorModule>> extractors;
    PKBStrategy* pkbStrategy;
public:
    explicit DesignExtractor(PKBStrategy* pkbStrategy) : pkbStrategy(pkbStrategy) {
        extractors.push_back(std::make_shared<StatementExtractorModule>(pkbStrategy));
        extractors.push_back(std::make_shared<VariableExtractorModule>(pkbStrategy));
        extractors.push_back(std::make_shared<ConstExtractorModule>(pkbStrategy));
        extractors.push_back(std::make_shared<ProcedureExtractorModule>(pkbStrategy));
        extractors.push_back(std::make_shared<ModifiesExtractorModule>(pkbStrategy));
        extractors.push_back(std::make_shared<UsesExtractorModule>(pkbStrategy));
        extractors.push_back(std::make_shared<FollowsExtractorModule>(pkbStrategy));
        extractors.push_back(std::make_shared<ParentExtractorModule>(pkbStrategy));
        extractors.push_back(std::make_shared<CallsExtractorModule>(pkbStrategy));
    }

    void extract(ast::ASTNode* ast) {
        for (auto extractor : extractors) {
            extractor->extract(ast);
        }
    }
};
}  // namespace design_extractor
}  // namespace sp
