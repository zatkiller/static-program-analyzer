#include <memory>

#include "SourceProcessor.h"
#include "PKB.h"
#include "Parser/Parser.h"
#include "DesignExtractor/ModifiesExtractor.h"
#include "DesignExtractor/VariableExtractor.h"
#include "DesignExtractor/StatementExtractor.h"

using SimpleParser::Parser;

bool SourceProcessor::processSimple(const std::string& sourceCode, PKB *pkb)
{
    auto ast = Parser().parse(sourceCode);
    // parsing failed
    if (!ast) {
        return false;
    }

    ast->accept(std::make_shared<VariableExtractor>(pkb));
    ast->accept(std::make_shared<ModifiesExtractor>(pkb));
    ast->accept(std::make_shared<StatementExtractor>(pkb));

    return true;
}

std::unique_ptr<AST::Program> SourceProcessor::parse(const std::string& sourceCode) {
    return Parser().parse(sourceCode);
}
