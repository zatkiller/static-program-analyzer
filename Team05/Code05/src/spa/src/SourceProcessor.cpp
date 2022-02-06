#include <memory>

#include "SourceProcessor.h"
#include "PKB.h"
#include "Parser/Parser.h"
#include "DesignExtractor/ModifiesExtractor.h"
#include "DesignExtractor/VariableExtractor.h"


using SimpleParser::Parser;

bool SourceProcessor::processSimple(const std::string& sourceCode)
{
	auto ast = Parser().parse(sourceCode);
	// parsing failed
	if (!ast) {
		return false;
	}


}

std::unique_ptr<AST::Program> SourceProcessor::parse(const std::string& sourceCode) {
	return Parser().parse(sourceCode);
}
