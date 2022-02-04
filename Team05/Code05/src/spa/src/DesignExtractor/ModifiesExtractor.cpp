#include "ModifiesExtractor.h"

void ModifiesExtractor::cascadeToContainer(const std::string& varName) {
    for (auto stmtNo : containerNumber) {
        pkb->insert("modifies", std::make_pair<>(stmtNo, varName));
    }
    if (!currentProcedureName.empty()) {
        pkb->insert("modifies", std::make_pair<>(currentProcedureName, varName));
    }
}

void ModifiesExtractor::visit(const AST::Read& node) {
    std::string varName = node.getVar().getVarName();
    pkb->insert("modifies", std::make_pair<>(node.getStmtNo(), varName));
    cascadeToContainer(varName);
}

void ModifiesExtractor::visit(const AST::Assign& node) {
    std::string varName = node.getLHS().getVarName();
    pkb->insert("modifies", std::make_pair<>(node.getStmtNo(), varName));
    cascadeToContainer(varName);
}

void ModifiesExtractor::enterContainer(std::variant<int, std::string> containerId) {
    int* pNum = std::get_if<int>(&containerId);
    if (pNum) {
        containerNumber.push_front(*pNum);
    } else {
        currentProcedureName = std::get<std::string>(containerId);
    }
}

void ModifiesExtractor::exitContainer() {
    containerNumber.pop_front();
}
