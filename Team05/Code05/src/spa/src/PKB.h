#pragma once

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include "PKB/StatementTable.h"
#include "PKB/VariableTable.h"
#include "PKB/ProcedureTable.h"
#include "PKB/ModifiesRelationshipTable.h"
#include "PKB/ConstantTable.h"
#include "PKB/PKBResponse.h"
#include "PKB/PKBReturnType.h"

typedef int PROC;

class TNode;

class VarTable;  // no need to #include "VarTable.h" as all I need is pointer

class PKB {
public:
	PKB();
	//static VarTable* varTable; 
	static int setProcToAST(PROC p, TNode* r);
	static TNode* getRootAST(PROC p);

	void insertStatement(StatementType, int);
	void insertRelationship(PKBRelationship, PKBField, PKBField);
	//void insertAST();

	PKBResponse getRelationship(PKBField, PKBField, PKBRelationship);
	PKBResponse getStatements(StatementType);
	PKBResponse getVariables();
	PKBResponse getProcedures();
	PKBResponse getConstants();

public:
	std::unique_ptr<StatementTable> statementTable;
	std::unique_ptr<VariableTable> variableTable;
	std::unique_ptr<ProcedureTable> procedureTable;
	std::unique_ptr<ModifiesRelationshipTable> modifiesTable;
	std::unique_ptr<ConstantTable> constantTable;
};
