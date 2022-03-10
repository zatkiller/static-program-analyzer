#pragma once

#include "DesignExtractor/Extractor.h"

namespace sp {
namespace design_extractor {

class CallsExtractor : public Extractor {
private:
    std::string currentProc = "";

public:
    using Extractor::Extractor;
    void visit(const ast::Procedure&) override;
    void visit(const ast::Call&) override;
};

}  // namespace design_extractor
}  // namespace sp
