#include "UsesExtractor.h"
#include "DesignExtractor/EntityExtractor/VariableExtractor.h"
#include "logging.h"

#define DEBUG_LOG Logger(Level::DEBUG) << "UsesExtractor.cpp Extracted "

void UsesExtractor::cascadeToContainer(const std::string& varName) {
    for (auto stmt : container) {
        // local table for testing
        table.insert(std::make_pair<>(stmt.statementNum, varName));
        DEBUG_LOG << "(" << stmt.statementNum << "," << varName << ")";
        pkb->insertRelationship(
            PKBRelationship::USES,
            PKBField::createConcrete(stmt),
            PKBField::createConcrete(VAR_NAME{varName})
        );
    }
    if (!currentProcedure.name.empty()) {
        // local table for testing
        table.insert(std::make_pair<>(currentProcedure.name, varName));
        DEBUG_LOG << "(" << currentProcedure.name << "," << varName << ")";
        pkb->insertRelationship(
            PKBRelationship::USES,
            PKBField::createConcrete(currentProcedure),
            PKBField::createConcrete(VAR_NAME{varName})
        );
    }
}

void UsesExtractor::extractAndInsert(STMT_LO stmt, const AST::ASTNode* part) {
    auto ve = std::make_shared<VariableExtractor_>();
    part->accept(ve);

    auto varNames = ve->getVars();

    for (auto varName : varNames) {
        table.insert(std::make_pair<>(stmt.statementNum, varName));
        DEBUG_LOG << "(" << stmt.statementNum << "," << varName << ")";
        pkb->insertRelationship(
            PKBRelationship::USES,
            PKBField::createConcrete(stmt),
            PKBField::createConcrete(VAR_NAME{varName})
        );
        cascadeToContainer(varName);
    }
}

void UsesExtractor::visit(const AST::Print& node) {
    extractAndInsert(STMT_LO{ node.getStmtNo(), StatementType::Print }, &node);
}

void UsesExtractor::visit(const AST::Assign& node) {
    extractAndInsert(STMT_LO{ node.getStmtNo(), StatementType::Assignment }, node.getRHS());
}

void UsesExtractor::visit(const AST::While& node) {
    stmtNumToType[node.getStmtNo()] = StatementType::While;
    extractAndInsert(STMT_LO{ node.getStmtNo(), StatementType::While }, node.getCondExpr());
}

void UsesExtractor::visit(const AST::If& node) {
    stmtNumToType[node.getStmtNo()] = StatementType::If;
    extractAndInsert(STMT_LO{ node.getStmtNo(), StatementType::If }, node.getCondExpr());
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
