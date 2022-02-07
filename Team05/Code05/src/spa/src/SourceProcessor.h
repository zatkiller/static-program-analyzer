#include <memory>

#include "Parser/AST.h"
#include "PKB.h"

class SourceProcessor {
    std::shared_ptr<PKB> pkb;
public:
    SourceProcessor(std::shared_ptr<PKB> pkb) : pkb(pkb) {};
    bool processSimple(const std::string&);
    std::unique_ptr<AST::Program> parse(const std::string&);
};
