#pragma once

#include <list>

#include "DesignExtractor/CFG/CFG.h"
#include "DesignExtractor/EntityExtractor/EntityExtractor.h"
#include "DesignExtractor/Extractor.h"
#include "DesignExtractor/RelationshipExtractor/RelationshipExtractor.h"
#include "Parser/AST.h"
#include "PKB.h"

namespace sp {
namespace design_extractor {
class DesignExtractor {
private:
    std::list<std::shared_ptr<ExtractorModule<const ast::ASTNode*>>> astExtractors;
    std::list<std::shared_ptr<ExtractorModule<const cfg::PROC_CFG_MAP*>>> cfgExtractors;
    PKB* pkb;
    cfg::PROC_CFG_MAP cfgs;

public:
    explicit DesignExtractor(PKB* pkb) : pkb(pkb) {
        astExtractors.push_back(std::make_shared<StatementExtractorModule>(pkb));
        astExtractors.push_back(std::make_shared<VariableExtractorModule>(pkb));
        astExtractors.push_back(std::make_shared<ConstExtractorModule>(pkb));
        astExtractors.push_back(std::make_shared<ProcedureExtractorModule>(pkb));
        astExtractors.push_back(std::make_shared<ModifiesExtractorModule>(pkb));
        astExtractors.push_back(std::make_shared<UsesExtractorModule>(pkb));
        astExtractors.push_back(std::make_shared<FollowsExtractorModule>(pkb));
        astExtractors.push_back(std::make_shared<ParentExtractorModule>(pkb));
        astExtractors.push_back(std::make_shared<CallsExtractorModule>(pkb));

        cfgExtractors.push_back(std::make_shared<NextExtractorModule>(pkb));
    }

    void extract(ast::ASTNode* ast) {
        sp::cfg::CFG cfgContainer;
        if (cfgs.empty()) {
            cfgContainer = cfg::CFGExtractor().extract(ast);
            cfgs = cfgContainer.cfgs;
        }
        for (auto extractor : astExtractors) {
            extractor->extract(ast);
        }
        for (auto extractor : cfgExtractors) {
            extractor->extract(&cfgs);
        }
    }

    void insert(const cfg::PROC_CFG_MAP &newCFGs) {
        cfgs = newCFGs;
    }
};
}  // namespace design_extractor
}  // namespace sp
