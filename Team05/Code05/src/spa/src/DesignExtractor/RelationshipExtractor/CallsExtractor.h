#pragma once

#include "DesignExtractor/Extractor.h"

namespace sp {
namespace design_extractor {
struct CallsExtractor : public Extractor {
    std::set<Entry> extract(const ast::ASTNode*) override;
};

struct CallsExtractorModule : public ExtractorModule {
public:
    CallsExtractorModule(PKBStrategy *pkb) : 
        ExtractorModule(std::make_unique<CallsExtractor>(), pkb) {};
};

}  // namespace design_extractor
}  // namespace sp
