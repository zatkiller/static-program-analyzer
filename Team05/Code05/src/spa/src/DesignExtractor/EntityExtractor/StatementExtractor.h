#include "DesignExtractor/Extractor.h"

namespace sp {
class StatementExtractor : public Extractor {
public:
    using Extractor::Extractor;
    void visit(const sp::ast::Read&) override;
    void visit(const sp::ast::Print&) override;
    void visit(const sp::ast::While&) override;
    void visit(const sp::ast::If&) override;
    void visit(const sp::ast::Assign&) override;
};
}
