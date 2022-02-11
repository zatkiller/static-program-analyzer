#include "UsesExtractor.h"
#include "DesignExtractor/EntityExtractor/VariableExtractor.h"
#include "logging.h"

#define DEBUG_LOG Logger(Level::DEBUG) << "UsesExtractor.cpp Extracted "

void UsesExtractor::cascadeToContainer(const std::string& varName) {
    for (auto stmt : container) {
        // local table for testing
        table.insert(std::make_pair<>(stmt.statementNum, varName));
        DEBUG_LOG << "(" << stmt.statementNum << "," << varName << ")";
    }
    if (!currentProcedure.name.empty()) {
        // local table for testing
        table.insert(std::make_pair<>(currentProcedure.name, varName));
        DEBUG_LOG << "(" << currentProcedure.name << "," << varName << ")";
    }
}

void UsesExtractor::extractAndInsert(int stmtNo, AST::ASTNode* part) {
    auto ve = std::make_shared<VariableExtractor_>();
    part->accept(ve);

    auto varNames = ve->getVars();

    for (auto varName : varNames) {
        table.insert(std::make_pair<>(stmtNo, varName));
        DEBUG_LOG << "(" << stmtNo << "," << varName << ")";
        cascadeToContainer(varName);
    }
}

void UsesExtractor::visit(const AST::Print& node) {
    std::string varName = node.getVar().getVarName();

    table.insert(std::make_pair<>(node.getStmtNo(), varName));
    DEBUG_LOG << "(" << node.getStmtNo() << "," << varName << ")";

    cascadeToContainer(varName);
}

void UsesExtractor::visit(const AST::Assign& node) {
    extractAndInsert(node.getStmtNo(), node.getRHS());
}

void UsesExtractor::visit(const AST::While& node) {
    stmtNumToType[node.getStmtNo()] = StatementType::While;
    extractAndInsert(node.getStmtNo(), node.getCondExpr());
}

void UsesExtractor::visit(const AST::If& node) {
    stmtNumToType[node.getStmtNo()] = StatementType::If;
    extractAndInsert(node.getStmtNo(), node.getCondExpr());
}

void UsesExtractor::enterContainer(std::variant<int, std::string> containerId) {
    int* pNum = std::get_if<int>(&containerId);
    if (pNum) {
        container.push_front(STMT_LO{ *pNum, stmtNumToType[*pNum] });
    } else {
        currentProcedure = PROC_NAME{ std::get<std::string>(containerId) };
    }
}

void UsesExtractor::exitContainer() {
    container.pop_front();
}
