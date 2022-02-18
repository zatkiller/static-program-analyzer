#include <memory>

#include "SourceProcessor.h"
#include "PKB.h"
#include "Parser/Parser.h"
#include "DesignExtractor/DesignExtractor.h"

bool SourceProcessor::processSimple(const std::string& sourceCode, PKB *pkb) {
    auto ast = sp::parser::parse(sourceCode);
    // parsing failed
    if (!ast) {
        return false;
    }

    sp::ActualPKBStrategy actualPKBStrategy(pkb);
    sp::DesignExtractor(&actualPKBStrategy).extract(ast.get());
    
    return true;
}

std::unique_ptr<sp::ast::Program> SourceProcessor::parse(const std::string& sourceCode) {
    return sp::parser::parse(sourceCode);
}
