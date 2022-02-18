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
#include "DesignExtractor/PatternMatcher.h"

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

    /**
    * Stores the AST parsed by the source processor.
    * 
    * @param root the pointer to the root of the AST of the SIMPLE source program
    */
    void insertAST(std::unique_ptr<AST::Program> root);

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
    * @return PKBResponse matching relationships wrapped in PKBResponse
    */
    PKBResponse getRelationship(PKBField field1, PKBField field2, PKBRelationship rs);

    /**
    * Retrieve all statements.
    *
    * @return PKBResponse all statements wrapped in PKBResponse
    */
    PKBResponse getStatements();

    /**
    * Retrieve all statements of a statement type.
    *
    * @param stmtType statement type
    * @return PKBResponse all statements of the given statement type wrapped in PKBResponse
    */
    PKBResponse getStatements(StatementType stmtType);

    /**
    * Retrieve all variables.
    *
    * @return PKBResponse all variables wrapped in PKBResponse
    */
    PKBResponse getVariables();

    /**
    * Retrieve all procedures.
    *
    * @return PKBResponse all procedures wrapped in PKBResponse
    */
    PKBResponse getProcedures();

    /**
    * Retrieve all constants.
    *
    * @return PKBResponse all constants wrapped in PKBResponse
    */
    PKBResponse getConstants();

    /**
    * @brief Retrieves all the statements of the provided type that satisfy the constraints given.
    
    * Currently it does not support strict matching like pattern a(v, "x") 
    * Only partial matching like pattern a(v, _"x+1"_) is supported. Only assignments are supported.
    *
    * For a(v, _"x+1_), use match(StatementType::Assignment, std::nullopt, "x+1")
    *
    * @param lhs The optional string constraint of LHS variable. Use std::nullopt if LHS is wildcard or synonym.
    * @param rhs The optional string constraint of RHS expression. Use std::nullopt if RHS  is wildcard.
    * @return PKBResponse 
    */
    PKBResponse match(StatementType type, PatternParam lhs, PatternParam rhs);

private:
    std::unique_ptr<StatementTable> statementTable;
    std::unique_ptr<VariableTable> variableTable;
    std::unique_ptr<ProcedureTable> procedureTable;
    std::unique_ptr<ModifiesRelationshipTable> modifiesTable;
    std::unique_ptr<FollowsRelationshipTable> followsTable;
    std::unique_ptr<ConstantTable> constantTable;
    std::unique_ptr<ParentRelationshipTable> parentTable;
    std::unique_ptr<UsesRelationshipTable> usesTable;
    std::unique_ptr<AST::ASTNode> root;

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
