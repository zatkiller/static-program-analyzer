#include <memory>

#include "SourceProcessor.h"
#include "PKB.h"
#include "Parser/Parser.h"
#include "DesignExtractor/ModifiesExtractor.h"
#include "DesignExtractor/VariableExtractor.h"


using SimpleParser::Parser;

bool SourceProcessor::processSimple(const std::string& sourceCode, PKB *pkb)
{
    auto ast = Parser().parse(sourceCode);
    // parsing failed
    if (!ast) {
        return false;
    }
    auto pkbAdaptor = std::make_shared<PKBAdaptor>(pkb);

    ast->accept(std::make_shared<VariableExtractor>(pkbAdaptor));
    ast->accept(std::make_shared<ModifiesExtractor>(pkbAdaptor));
}

std::unique_ptr<AST::Program> SourceProcessor::parse(const std::string& sourceCode) {
    return Parser().parse(sourceCode);
}
