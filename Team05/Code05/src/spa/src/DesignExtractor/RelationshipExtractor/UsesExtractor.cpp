#include "UsesExtractor.h"
#include "DesignExtractor/EntityExtractor/VariableExtractor.h"
#include "logging.h"

#define DEBUG_LOG Logger(Level::DEBUG) << "UsesExtractor.cpp Extracted "

/**
 * Private strategy to only retrieve a local copy of variables.
 */
class VariablePKBStrategy : public NullPKBStrategy {
public:
    std::set<std::string> variables;
    void insertVariable(std::string name) override {
        variables.insert(name);
    };
};

void UsesExtractor::cascadeToContainer(const std::string& varName) {
    for (auto stmt : container) {
        DEBUG_LOG << "(" << stmt.statementNum << "," << varName << ")";
        pkb->insertRelationship(
            PKBRelationship::USES,
            stmt,
            VAR_NAME{varName}
        );
    }
    if (!currentProcedure.name.empty()) {
        DEBUG_LOG << "(" << currentProcedure.name << "," << varName << ")";
        pkb->insertRelationship(
            PKBRelationship::USES,
            currentProcedure,
            VAR_NAME{varName}
        );
    }
}

void UsesExtractor::extractAndInsert(STMT_LO stmt, const AST::ASTNode* part) {
    VariablePKBStrategy vps;
    auto ve = std::make_shared<VariableExtractor>(&vps);
    part->accept(ve);

    auto varNames = vps.variables;

    for (auto varName : varNames) {
        DEBUG_LOG << "(" << stmt.statementNum << "," << varName << ")";
        pkb->insertRelationship(
            PKBRelationship::USES,
            stmt,
            VAR_NAME{varName}
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
