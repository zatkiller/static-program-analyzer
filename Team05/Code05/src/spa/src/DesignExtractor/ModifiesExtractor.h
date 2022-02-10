#pragma once

#include <deque>
#include <string>

#include "Parser/AST.h"
#include "DesignExtractor.h"


/**
 * Extracts all modifies relationship from the AST and send them to PKB Adaptor.
 */
class ModifiesExtractor : public Extractor {
private:
    std::deque<int> containerNumber;
    std::string currentProcedureName;
    muTable table;

    /**
     * Cascade the modifies relationship up the container stack. If a container contains a modify statement
     * that modifies x, then the container itself modifies x.
     *
     * @param varName the name of the variable that is modified.
     */
    void cascadeToContainer(const std::string& varName);

public:
    using Extractor::Extractor;

    void visit(const AST::Read& node) override;
    void visit(const AST::Assign& node) override;
    void enterContainer(std::variant<int, std::string> containerId) override;
    void exitContainer() override;

    muTable getModifies() {
        return table;
    }
};
