#pragma once

#include <list>

#include "PKB.h"
#include "Parser/AST.h"
#include "DesignExtractor/Extractor.h"
#include "DesignExtractor/CFG/CFG.h"
#include "DesignExtractor/EntityExtractor/EntityExtractor.h"
#include "DesignExtractor/RelationshipExtractor/RelationshipExtractor.h"

namespace sp {
namespace design_extractor {
class DesignExtractor {
private:
    std::list<std::shared_ptr<ExtractorModule<const ast::ASTNode*>>> astExtractors;
    std::list<std::shared_ptr<ExtractorModule<const cfg::PROC_CFG_MAP*>>> cfgExtractors;
    PKBStrategy* pkbStrategy;
    ProcToCfgMap cfgs;

public:
    explicit DesignExtractor(PKBStrategy* pkbStrategy) : pkbStrategy(pkbStrategy) {
        astExtractors.push_back(std::make_shared<StatementExtractorModule>(pkbStrategy));
        astExtractors.push_back(std::make_shared<VariableExtractorModule>(pkbStrategy));
        astExtractors.push_back(std::make_shared<ConstExtractorModule>(pkbStrategy));
        astExtractors.push_back(std::make_shared<ProcedureExtractorModule>(pkbStrategy));
        astExtractors.push_back(std::make_shared<ModifiesExtractorModule>(pkbStrategy));
        astExtractors.push_back(std::make_shared<UsesExtractorModule>(pkbStrategy));
        astExtractors.push_back(std::make_shared<FollowsExtractorModule>(pkbStrategy));
        astExtractors.push_back(std::make_shared<ParentExtractorModule>(pkbStrategy));
        astExtractors.push_back(std::make_shared<CallsExtractorModule>(pkbStrategy));

        cfgExtractors.push_back(std::make_shared<NextExtractorModule>(pkbStrategy));
    }

    void extract(ast::ASTNode* ast) {
        if (cfgs.empty()) {
            auto cfgs = cfg::CFGExtractor().extract(ast);
        }
        for (auto extractor : astExtractors) {
            extractor->extract(ast);
        }
        for (auto extractor : cfgExtractors) {
            extractor->extract(&cfgs);
        }
    }

    void insert(const ProcToCfgMap &newCFGs) {
        cfgs = newCFGs;
    }
};
}  // namespace design_extractor
}  // namespace sp
