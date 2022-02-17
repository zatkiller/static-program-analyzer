#pragma once

#include <list>

#include "PKB.h"
#include "Parser/AST.h"
#include "DesignExtractor/Extractor.h"
#include "DesignExtractor/EntityExtractor/StatementExtractor.h"
#include "DesignExtractor/EntityExtractor/VariableExtractor.h"
#include "DesignExtractor/RelationshipExtractor/ModifiesExtractor.h"
#include "DesignExtractor/RelationshipExtractor/UsesExtractor.h"
#include "DesignExtractor/RelationshipExtractor/FollowsExtractor.h"
#include "DesignExtractor/RelationshipExtractor/ParentExtractor.h"


class DesignExtractor {
private:
    std::list<std::shared_ptr<Extractor>> extractors;
    PKBStrategy* pkbStrategy;
public:
    explicit DesignExtractor(PKBStrategy* pkbStrategy) : pkbStrategy(pkbStrategy) {
        extractors.push_back(std::make_shared<StatementExtractor>(pkbStrategy));
        extractors.push_back(std::make_shared<VariableExtractor>(pkbStrategy));
        extractors.push_back(std::make_shared<ModifiesExtractor>(pkbStrategy));
        extractors.push_back(std::make_shared<UsesExtractor>(pkbStrategy));
        extractors.push_back(std::make_shared<FollowsExtractor>(pkbStrategy));
        extractors.push_back(std::make_shared<ParentExtractor>(pkbStrategy));
    }

    void extract(AST::ASTNode* ast) {
        for (auto extractor : extractors) {
            ast->accept(extractor);
        }
    }
};
