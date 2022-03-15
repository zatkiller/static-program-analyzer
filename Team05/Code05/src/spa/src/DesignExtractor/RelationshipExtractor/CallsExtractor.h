#pragma once

#include "DesignExtractor/Extractor.h"

namespace sp {
namespace design_extractor {

class CallsCollector : public TreeWalker {
private:
    std::string currentProc = "";
public:
    std::set<Entry> relationships;
    void visit(const ast::Procedure&) override;
    void visit(const ast::Call&) override;
};

struct CallsExtractor : public IExtractor {
    std::set<Entry> extract(const ast::ASTNode*) override;
};

struct CallsExtractorModule : public ExtractorModule {
public:
    CallsExtractorModule(PKBStrategy *pkb) : 
        ExtractorModule(std::make_unique<CallsExtractor>(), pkb) {};
};

}  // namespace design_extractor
}  // namespace sp
