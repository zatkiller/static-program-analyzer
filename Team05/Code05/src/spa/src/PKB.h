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
#include "PKB/FollowsRelationshipTable.h"
#include "PKB/ParentRelationshipTable.h"
#include "PKB/UsesRelationshipTable.h"
#include "PKB/ConstantTable.h"
#include "PKB/PKBResponse.h"
#include "PKB/PKBReturnType.h"
#include "PKB/PKBField.h"

typedef int PROC;

class TNode;

class VarTable;  // no need to #include "VarTable.h" as all I need is pointer

class PKB {
public:
    PKB();
    // static VarTable* varTable; 
    static int setProcToAST(PROC p, TNode* r);
    static TNode* getRootAST(PROC p);

    /**
    * Inserts a statement information into the PKB.
    *
    * @param type statement type
    * @param statementNumber line number
    */
    void insertStatement(StatementType type, int statementNumber);

    /**
    * Inserts a variable into the PKB.
    *
    * @param name variable name
    */
    void insertVariable(std::string name);

    /**
    * Inserts a relationship into the PKB.
    *
    * @param type relationship type
    * @param field1 the first program design entity in the relationship
    * @param field2 the second program design entity in the relationship
    */
    void insertRelationship(PKBRelationship type, PKBField field1, PKBField field2);

    // void insertAST();

    /**
    * Checks whether there exist. If any fields are invalid, return false.
    * 
    * @param field1 the first program design entity in the relationship
    * @param field2 the second program design entity in the relationship
    * @param rs the relationship type 
    * 
    * @return bool
    */
    bool isRelationshipPresent(PKBField field1, PKBField field2, PKBRelationship rs);

    /**
    * Retrieve all relationships matching rs(field1, field2). If any fields are invalid, 
    * an empty PKBResponse is returned.
    *
    * @param field1 the first program design entity in the relationship
    * @param field2 the second program design entity in the relationship
    * @param rs the relationship type 
    * 
    * @return matching relationships wrapped in PKBResponse
    */
    PKBResponse getRelationship(PKBField field1, PKBField field2, PKBRelationship rs);

    /**
    * Retrieve all statements.
    *
    * @return all statements wrapped in PKBResponse
    */
    PKBResponse getStatements();

    /**
    * Retrieve all statements of a statement type.
    *
    * @param stmtType statement type
    * @return all statements of the given statement type wrapped in PKBResponse
    */
    PKBResponse getStatements(StatementType stmtType);

    /**
    * Retrieve all variables.
    *
    * @return all variables wrapped in PKBResponse
    */
    PKBResponse getVariables();

    /**
    * Retrieve all procedures.
    *
    * @return all procedures wrapped in PKBResponse
    */
    PKBResponse getProcedures();

    /**
    * Retrieve all constants.
    *
    * @return all constants wrapped in PKBResponse
    */
    PKBResponse getConstants();

private:
    std::unique_ptr<StatementTable> statementTable;
    std::unique_ptr<VariableTable> variableTable;
    std::unique_ptr<ProcedureTable> procedureTable;
    std::unique_ptr<ModifiesRelationshipTable> modifiesTable;
    std::unique_ptr<FollowsRelationshipTable> followsTable;
    std::unique_ptr<ConstantTable> constantTable;
    std::unique_ptr<ParentRelationshipTable> parentTable;
    std::unique_ptr<UsesRelationshipTable> usesTable;

    /**
    * Check whether field is a concrete statement. Returns false if it is a 
    * concrete statement with an uninitialized statement type and its statement number is absent in
    * the StatementTable. 
    * 
    * @param field
    * @return bool
    */
    bool getStatementTypeOfConcreteField(PKBField* field);
};
