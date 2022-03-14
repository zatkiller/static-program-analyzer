#pragma once

#include "DesignExtractor/Extractor.h"


namespace sp {
namespace design_extractor {
class ConstExtractor : public IExtractor {
public:
    using IExtractor::IExtractor;
    std::set<Entry> extract(const ast::ASTNode*) override;
};

class ConstExtractorModule : public ExtractorModule {
public:
    ConstExtractorModule(PKBStrategy *pkb) : 
        ExtractorModule(std::make_unique<ConstExtractor>(), pkb) {};
};
}  // namespace design_extractor
}  // namespace sp
