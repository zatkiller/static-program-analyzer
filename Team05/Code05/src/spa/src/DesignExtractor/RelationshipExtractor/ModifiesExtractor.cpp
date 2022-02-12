#include "ModifiesExtractor.h"
#include  "logging.h"

#define DEBUG_LOG Logger(Level::DEBUG) << "ModifiesExtractor.cpp Extracted "

void ModifiesExtractor::cascadeToContainer(const std::string& varName) {
    for (auto stmt : container) {
        // local table for testing
        table.insert(std::make_pair<>(stmt.statementNum, varName));
        DEBUG_LOG << "(" << stmt.statementNum << "," << varName << ")";

        pkb->insertRelationship(
            PKBRelationship::MODIFIES,
            stmt,
            VAR_NAME{ varName }
        );
    }
    if (!currentProcedure.name.empty()) {
        // local table for testing
        table.insert(std::make_pair<>(currentProcedure.name, varName));
        DEBUG_LOG << "(" << currentProcedure.name << "," << varName << ")";

        pkb->insertRelationship(
            PKBRelationship::MODIFIES,
            currentProcedure,
            VAR_NAME{ varName }
        );
    }
}

void ModifiesExtractor::visit(const AST::Read& node) {
    std::string varName = node.getVar().getVarName();

    table.insert(std::make_pair<>(node.getStmtNo(), varName));
    DEBUG_LOG << "(" << node.getStmtNo() << "," << varName << ")";

    pkb->insertRelationship(
        PKBRelationship::MODIFIES,
        STMT_LO{ node.getStmtNo(), StatementType::Read},
        VAR_NAME{ varName }
    );

    cascadeToContainer(varName);
}

void ModifiesExtractor::visit(const AST::Assign& node) {
    std::string varName = node.getLHS()->getVarName();

    table.insert(std::make_pair<>(node.getStmtNo(), varName));
    DEBUG_LOG << "(" << node.getStmtNo() << "," << varName << ")";

    pkb->insertRelationship(
        PKBRelationship::MODIFIES,
        STMT_LO{ node.getStmtNo(), StatementType::Assignment },
        VAR_NAME{ varName }
    );

    cascadeToContainer(varName);
}

void ModifiesExtractor::visit(const AST::While& node) {
    stmtNumToType[node.getStmtNo()] = StatementType::While;
}

void ModifiesExtractor::visit(const AST::If& node) {
    stmtNumToType[node.getStmtNo()] = StatementType::If;
}

void ModifiesExtractor::enterContainer(std::variant<int, std::string> containerId) {
    int* pNum = std::get_if<int>(&containerId);
    if (pNum) {
        container.push_front(STMT_LO{ *pNum, stmtNumToType[*pNum] });
    } else {
        currentProcedure = PROC_NAME{ std::get<std::string>(containerId) };
    }
}

void ModifiesExtractor::exitContainer() {
    container.pop_front();
}
