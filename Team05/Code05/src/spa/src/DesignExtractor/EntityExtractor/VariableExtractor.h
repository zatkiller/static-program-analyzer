#pragma once

#include <string>

#include "DesignExtractor/Extractor.h"


/**
 * Extracts all variables from the AST and send them to PKB Adaptor.
 */
class VariableExtractor : public Extractor {
private:
    sTable table;

public:
    using Extractor::Extractor;
    void visit(const AST::Var& node) override;
    sTable getVars() {
        return table;
    }
};

/**
 * Private implementation of variable extraction without PKB interaction.
 */
class VariableExtractor_ : public TreeWalker {
private:
    sTable table;

public:
    void visit(const AST::Var& node) override {
        table.insert(node.getVarName());
    };
    sTable getVars() {
        return table;
    }
};