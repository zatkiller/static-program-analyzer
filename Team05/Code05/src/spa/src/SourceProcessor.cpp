#include <memory>

#include "SourceProcessor.h"
#include "PKB.h"
#include "Parser/Parser.h"
#include "DesignExtractor/DesignExtractor.h"

bool SourceProcessor::processSimple(const std::string& sourceCode, PKB *pkb) {
    auto ast = SimpleParser::parse(sourceCode);
    // parsing failed
    if (!ast) {
        return false;
    }

    ActualPKBStrategy actualPKBStrategy(pkb);
    DesignExtractor(&actualPKBStrategy).extract(ast.get());
    
    return true;
}

std::unique_ptr<AST::Program> SourceProcessor::parse(const std::string& sourceCode) {
    return SimpleParser::parse(sourceCode);
}
