#pragma once

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>

#include "PKB/StatementTable.h"
#include "PKB/VariableTable.h"
#include "PKB/ProcedureTable.h"
#include "PKB/ModifiesRelationshipTable.h"

typedef int PROC;

class TNode;

class VarTable;  // no need to #include "VarTable.h" as all I need is pointer

class PKB {
public:
    PKB();
    static VarTable* varTable; 
    static int setProcToAST(PROC p, TNode* r);
    static TNode* getRootAST(PROC p);

    void insertStatement(StatementType, int);
    void insertRelationship(PKBRelationship, PKBField, PKBField);
    //void insertAST();

private:
    static StatementTable* statementTable;
    static VariableTable* variableTable;
    static ProcedureTable* procedureTable;
    static ModifiesRelationshipTable* modifiesTable;
};
