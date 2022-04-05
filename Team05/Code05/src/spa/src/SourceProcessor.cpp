#include <memory>

#include "SourceProcessor.h"
#include "PKB.h"
#include "DesignExtractor/CFG/CFG.h"
#include "Parser/Parser.h"
#include "DesignExtractor/DesignExtractor.h"

bool SourceProcessor::processSimple(const std::string& sourceCode, PKB *pkb) {
    using sp::design_extractor::ActualPKBStrategy;
    using sp::design_extractor::DesignExtractor;

    // parsing source code and extracting AST
    auto ast = sp::parser::parse(sourceCode);
    // parsing failed
    if (!ast) {
        return false;
    }

    // extracting CFG
    sp::cfg::CFGExtractor cfgExtractor;
    sp::cfg::PROC_CFG_MAP cfgs = cfgExtractor.extract(ast.get());

    // setting up Design Extractor
    ActualPKBStrategy actualPKBStrategy(pkb);
    auto de = DesignExtractor(&actualPKBStrategy);
    de.insert(cfgs);  // insert CFG to Design Extractor for Next Extractor
    
    // running extractors and inserting into PKB
    de.extract(ast.get());

    // inserting CFG and AST into PKB
    pkb->insertCFG(cfgs);
    pkb->insertAST(std::move(ast));
    
    return true;
}

std::unique_ptr<sp::ast::Program> SourceProcessor::parse(const std::string& sourceCode) {
    return sp::parser::parse(sourceCode);
}
