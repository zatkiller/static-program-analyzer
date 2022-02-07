#include "DesignExtractor.h"

class StatementExtractor : public Extractor {
public:
    using Extractor::Extractor;
    void visit(const AST::Read&) override;
    void visit(const AST::Print&) override;
    void visit(const AST::While&) override;
    void visit(const AST::If&) override;
    void visit(const AST::Assign&) override;
};
