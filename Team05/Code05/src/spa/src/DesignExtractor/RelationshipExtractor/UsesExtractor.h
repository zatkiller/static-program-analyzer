#pragma once

#include <deque>
#include <string>

#include "Parser/AST.h"
#include "DesignExtractor/Extractor.h"
#include "PKB.h"


/**
 * Extracts all uses relationship from the AST and send them to PKB Adaptor.
 */
class UsesExtractor : public Extractor {
private:
    std::deque<STMT_LO> container;
    PROC_NAME currentProcedure = PROC_NAME{ "" };
    muTable table;
    std::map<int, StatementType> stmtNumToType;

    /**
     * Cascade the uses relationship up the container stack. If a container contains a use statement
     * that uses x, then the container itself uses x.
     *
     * @param varName the name of the variable that is modified.
     */
    void cascadeToContainer(const std::string& varNames);
    void extractAndInsert(STMT_LO, const AST::ASTNode*);

public:
    using Extractor::Extractor;

    void visit(const AST::Print& node) override;
    void visit(const AST::Assign& node) override;
    void visit(const AST::While& node) override;
    void visit(const AST::If& node) override;
    void enterContainer(std::variant<int, std::string> containerId) override;
    void exitContainer() override;

    /**
     * Returns a muTable that contains a local copy of uses relationship extracted.
     * Mainly used for testing purposes.
     */
    muTable getUses() {
        return table;
    }
};
