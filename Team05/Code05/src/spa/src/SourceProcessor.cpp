#include <memory>

#include "SourceProcessor.h"
#include "PKB.h"
#include "Parser/Parser.h"
#include "DesignExtractor/DesignExtractor.h"

bool SourceProcessor::processSimple(const std::string& sourceCode, PKB *pkb) {
    using sp::design_extractor::ActualPKBStrategy;
    using sp::design_extractor::DesignExtractor;

    auto ast = sp::parser::parse(sourceCode);
    // parsing failed
    if (!ast) {
        return false;
    }

    ActualPKBStrategy actualPKBStrategy(pkb);
    DesignExtractor(&actualPKBStrategy).extract(ast.get());
    pkb->insertAST(std::move(ast));
    return true;
}

std::unique_ptr<sp::ast::Program> SourceProcessor::parse(const std::string& sourceCode) {
    return sp::parser::parse(sourceCode);
}
