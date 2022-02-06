#include <memory>

#include "Parser/AST.h"
#include "PKB.h"

class SourceProcessor {
	PKB* pkb;
public:
	SourceProcessor(PKB* pkb) : pkb(pkb) {};
	bool processSimple(const std::string&);
	std::unique_ptr<AST::Program> parse(const std::string&);
};
