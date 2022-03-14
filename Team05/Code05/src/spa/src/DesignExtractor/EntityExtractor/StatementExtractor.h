#include "DesignExtractor/Extractor.h"

namespace sp {
namespace design_extractor {

class StatementExtractor : public IExtractor {
public:
    using IExtractor::IExtractor;
    std::set<Entry> extract(const ast::ASTNode*) override;
};

class StatementExtractorModule : public ExtractorModule {
public:
    StatementExtractorModule(PKBStrategy *pkb) : 
        ExtractorModule(std::make_unique<StatementExtractor>(), pkb) {};
};
}  // namespace design_extractor
}  // namespace sp
