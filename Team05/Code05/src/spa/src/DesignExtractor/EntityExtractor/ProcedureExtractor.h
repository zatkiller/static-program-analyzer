#pragma once

#include "DesignExtractor/Extractor.h"


namespace sp {
namespace design_extractor {

class ProcedureExtractor : public IExtractor {
public:
    using IExtractor::IExtractor;
    std::set<Entry> extract(const ast::ASTNode*) override;
};

class ProcedureExtractorModule : public ExtractorModule {
public:
    ProcedureExtractorModule(PKBStrategy *pkb) : 
        ExtractorModule(std::make_unique<ProcedureExtractor>(), pkb) {};
};

}  // namespace design_extractor
}  // namespace sp
