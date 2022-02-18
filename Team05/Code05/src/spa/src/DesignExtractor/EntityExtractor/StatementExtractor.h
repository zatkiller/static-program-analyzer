#include "DesignExtractor/Extractor.h"

namespace sp {
namespace design_extractor {
class StatementExtractor : public Extractor {
public:
    using Extractor::Extractor;
    void visit(const ast::Read&) override;
    void visit(const ast::Print&) override;
    void visit(const ast::While&) override;
    void visit(const ast::If&) override;
    void visit(const ast::Assign&) override;
};
}  // namespace design_extractor
}  // namespace sp
