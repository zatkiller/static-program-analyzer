#include "ModifiesExtractor.h"
#include  "logging.h"

#define DEBUG_LOG Logger(Level::DEBUG) << "ModifiesExtractor.cpp Extracted "

void ModifiesExtractor::cascadeToContainer(const std::string& varName) {
    for (auto stmtNo : containerNumber) {
        // local table for testing
        table.insert(std::make_pair<>(stmtNo, varName));
        DEBUG_LOG << "(" << stmtNo << "," << varName << ")";

        pkb->insertRelationship(
            PKBRelationship::MODIFIES,
            PKBField{ PKBType::STATEMENT, true, STMT_LO{stmtNo} },
            PKBField{ PKBType::VARIABLE, true, VAR_NAME{varName} }
        );
    }
    if (!currentProcedureName.empty()) {
        // local table for testing
        table.insert(std::make_pair<>(currentProcedureName, varName));
        DEBUG_LOG << "(" << currentProcedureName << "," << varName << ")";

        pkb->insertRelationship(
            PKBRelationship::MODIFIES,
            PKBField{ PKBType::PROCEDURE, true, PROC_NAME{currentProcedureName} },
            PKBField{ PKBType::VARIABLE, true, VAR_NAME{varName} }
        );
    }
}

void ModifiesExtractor::visit(const AST::Read& node) {
    std::string varName = node.getVar().getVarName();

    table.insert(std::make_pair<>(node.getStmtNo(), varName));
    DEBUG_LOG << "(" << node.getStmtNo() << "," << varName << ")";

    pkb->insertRelationship(
        PKBRelationship::MODIFIES,
        PKBField{ PKBType::STATEMENT, true, STMT_LO{node.getStmtNo()} },
        PKBField{ PKBType::VARIABLE, true, VAR_NAME{varName} }
    );

    cascadeToContainer(varName);
}

void ModifiesExtractor::visit(const AST::Assign& node) {
    std::string varName = node.getLHS().getVarName();

    table.insert(std::make_pair<>(node.getStmtNo(), varName));
    DEBUG_LOG << "(" << node.getStmtNo() << "," << varName << ")";

    pkb->insertRelationship(
        PKBRelationship::MODIFIES,
        PKBField{ PKBType::STATEMENT, true, STMT_LO{node.getStmtNo()} },
        PKBField{ PKBType::VARIABLE, true, VAR_NAME{varName} }
    );

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
