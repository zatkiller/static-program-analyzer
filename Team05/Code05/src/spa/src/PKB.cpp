#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>

#include "logging.h"
#include "PKB.h"
#include "TNode.h"

PKB::PKB() {
    statementTable = std::unique_ptr<StatementTable>(new StatementTable());
    variableTable = std::unique_ptr<VariableTable>(new VariableTable());
    procedureTable = std::unique_ptr<ProcedureTable>(new ProcedureTable());
    modifiesTable = std::unique_ptr<ModifiesRelationshipTable>(new ModifiesRelationshipTable());
}

int PKB::setProcToAST(PROC p, TNode* r) {
    return 0;
}

TNode* PKB::getRootAST(PROC p) {
    return nullptr;
}

// INSERT API

void PKB::insertStatement(StatementType type, int statementNumber) {
    statementTable->insert(type, statementNumber);
}

void PKB::insertRelationship(PKBRelationship type, PKBField entity1, PKBField entity2) {
    switch (type) {
    case PKBRelationship::MODIFIES:
        modifiesTable->insert(entity1, entity2);
        break;
    default:
        Logger(Level::INFO) << "Inserted into an invalid relationship table\n";
    }
}

// GET API

PKBResponse PKB::getRelationship(PKBField field1, PKBField field2, PKBRelationship rs) {
    switch (rs) {
    case PKBRelationship::MODIFIES:
    {
        std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> extracted = 
            modifiesTable->retrieve(field1, field2);
        return extracted.size() != 0 
            ? PKBResponse{ true, Response{extracted} } 
            : PKBResponse{ false, Response{extracted} };
        break;
    }
    default:
        throw "Invalid relationship type used!";
    }
}

PKBResponse PKB::getStatements(StatementType stmtType) {
    std::unordered_set<PKBField, PKBFieldHash> res;

    std::vector<STMT_LO> extracted = statementTable->getStmtOfType(stmtType);
    for (auto iter = extracted.begin(); iter != extracted.end(); ++iter) {
        PKBField temp{ PKBType::STATEMENT, true, Content{ *iter } };
        res.insert(temp);
    }

    return res.size() != 0 ? PKBResponse{ true, Response{res} } : PKBResponse{ false, Response{res} };
}

PKBResponse PKB::getVariables() {
    std::unordered_set<PKBField, PKBFieldHash> res;

    std::vector<VAR_NAME> extracted = variableTable->getAllVars();
    for (auto iter = extracted.begin(); iter != extracted.end(); ++iter) {
        PKBField temp{ PKBType::VARIABLE, true, Content{ *iter } };
        res.insert(temp);
    }
    
    return res.size() != 0 ? PKBResponse{ true, Response{res} } : PKBResponse{ false, Response{res} };
}

PKBResponse PKB::getProcedures() {
    std::unordered_set<PKBField, PKBFieldHash> res;

    std::vector<PROC_NAME> extracted = procedureTable->getAllProcs();
    for (auto iter = extracted.begin(); iter != extracted.end(); ++iter) {
        PKBField temp{ PKBType::PROCEDURE, true, Content{ *iter } };
        res.insert(temp);
    }

    return res.size() != 0 ? PKBResponse{ true, Response{res} } : PKBResponse{ false, Response{res} };
}

PKBResponse PKB::getConstants() {
    std::unordered_set<PKBField, PKBFieldHash> res;

    std::vector<CONST> extracted = constantTable->getAllConst();
    for (auto iter = extracted.begin(); iter != extracted.end(); ++iter) {
        PKBField temp{ PKBType::CONST, true, Content{ *iter } };
        res.insert(temp);
    }

    return res.size() != 0 ? PKBResponse{ true, Response{res} } : PKBResponse{ false, Response{res} };
}
