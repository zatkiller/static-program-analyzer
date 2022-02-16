#pragma once

#include <memory>
#include <string>

#include "Parser/AST.h"
#include "PKB.h"

class SourceProcessor {
public:
    SourceProcessor() {}
    bool processSimple(const std::string&, PKB*);
    std::unique_ptr<AST::Program> parse(const std::string&);
};
