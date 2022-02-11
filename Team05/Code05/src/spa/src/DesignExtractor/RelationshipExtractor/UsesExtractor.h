#pragma once

#include <deque>
#include <string>

#include "Parser/AST.h"
#include "DesignExtractor/Extractor.h"
#include "PKB.h"


/**
 * Extracts all modifies relationship from the AST and send them to PKB Adaptor.
 */
class UsesExtractor : public Extractor {
private:
    std::deque<STMT_LO> container;
    PROC_NAME currentProcedure = PROC_NAME{ "" };
    muTable table;
    std::map<int, StatementType> stmtNumToType;

    /**
     * Cascade the modifies relationship up the container stack. If a container contains a modify statement
     * that modifies x, then the container itself modifies x.
     *
     * @param varName the name of the variable that is modified.
     */
    void cascadeToContainer(const std::string& varNames);

public:
    using Extractor::Extractor;

    void visit(const AST::Print& node) override;
    void visit(const AST::Assign& node) override;
    void visit(const AST::While& node) override;
    void visit(const AST::If& node) override;
    void enterContainer(std::variant<int, std::string> containerId) override;
    void exitContainer() override;

    muTable getUses() {
        return table;
    }
};
