#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>

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

TNode* PKB::getRootAST (PROC p) {
    return nullptr;
}

void PKB::insertStatement(StatementType type, int statementNumber) {
    statementTable->insert(type, statementNumber);
}

void PKB::insertRelationship(PKBRelationship type, PKBField entity1, PKBField entity2) {
    switch (type) {
    case PKBRelationship::MODIFIES:
        modifiesTable->insert(entity1, entity2);
        break;
    default:
        std::cerr << "Insert into an invalid relationship table\n";
    }
}
