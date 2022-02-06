#pragma once

#include <string>

#include "DesignExtractor.h"


/**
 * Extracts all variables from the AST and send them to PKB Adaptor.
 */
class VariableExtractor : public Extractor {
public:
    using Extractor::Extractor;
    void visit(const AST::Var& node) override;
    sTable getVars() {
        return std::get<std::set<std::string>>(pkb->tables["variables"]);
    }
};
