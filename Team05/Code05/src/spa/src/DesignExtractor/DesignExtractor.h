#pragma once

#include <list>

#include "PKB.h"
#include "Parser/AST.h"
#include "DesignExtractor/Extractor.h"
#include "DesignExtractor/EntityExtractor/StatementExtractor.h"
#include "DesignExtractor/EntityExtractor/VariableExtractor.h"
#include "DesignExtractor/RelationshipExtractor/ModifiesExtractor.h"


class DesignExtractor {
private:
    std::list<std::shared_ptr<Extractor>> extractors;
    PKBStrategy* pkb;
public:
    DesignExtractor(PKBStrategy* pkb) : pkb(pkb) {
        extractors.push_back(std::make_shared<StatementExtractor>(pkb));
        extractors.push_back(std::make_shared<VariableExtractor>(pkb));
        extractors.push_back(std::make_shared<ModifiesExtractor>(pkb));
    };

    void extract(AST::ASTNode* ast) {
        for (auto extractor : extractors) {
            ast->accept(extractor);
        }
    }
};
