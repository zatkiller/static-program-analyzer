#pragma once

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "logging.h"

#include "PKB/PKBTables.h"
#include "PKB/PKBResponse.h"
#include "PKB/PKBField.h"
#include "DesignExtractor/PatternMatcher.h"

class PKB {
public:
    PKB();

    /**
    * Inserts a program design entity into its respective table
    *
    * @param entity a STMT_LO, PROC_NAME, VAR_NAME, or CONST
    */
    void insertEntity(Content entity);

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
    void insertAST(std::unique_ptr<sp::ast::Program> root);

    /**
    * Checks whether there exist. If any fields are invalid, return false. Both fields must be concrete.
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
    * @brief Retrieves all the statements of the provided type that satisfy the constraints given
    * Only assignments are supported.
    *
    * For a(v, _"x+1"_), use match(StatementType::Assignment, std::nullopt, "x+1")
    * For a(v, "x+1"), use match(StatementType::Assignment, std::nullopt, "x+1", true)
    *
    * @param lhs The optional string constraint of LHS variable. Use std::nullopt if LHS is wildcard or synonym.
    * @param rhs The optional string constraint of RHS expression. Use std::nullopt if RHS is wildcard.
    * @param isStrict An optional boolean that indicates whether strict pattern matching should be used.
    *   Defaults to false if no argument provided.
    * @return PKBResponse
    */
    PKBResponse match(
        StatementType type,
        sp::design_extractor::PatternParam lhs,
        sp::design_extractor::PatternParam rhs,
        bool isStrict = false
    );

private:
    std::unique_ptr<StatementTable> statementTable;
    std::unique_ptr<VariableTable> variableTable;
    std::unique_ptr<ProcedureTable> procedureTable;
    std::unique_ptr<ModifiesRelationshipTable> modifiesTable;
    std::unique_ptr<FollowsRelationshipTable> followsTable;
    std::unique_ptr<ConstantTable> constantTable;
    std::unique_ptr<ParentRelationshipTable> parentTable;
    std::unique_ptr<UsesRelationshipTable> usesTable;
    std::unique_ptr<CallsRelationshipTable> callsTable;
    std::unique_ptr<NextRelationshipTable> nextTable;
    std::unique_ptr<sp::ast::ASTNode> root;
    std::unordered_map<PKBRelationship, std::shared_ptr<RelationshipTable>> relationshipTables;

    std::shared_ptr<RelationshipTable> getRelationshipTable(PKBRelationship relationship) const;
    /**
   * Inserts an assignment, if, or while statement information into the PKB.
   *
   * @param stmt
   */
    void insertStatement(STMT_LO stmt);

    /**
    * Inserts a variable into the PKB.
    *
    * @param var
    */
    void insertVariable(VAR_NAME var);

    /**
    * Inserts a constant into the PKB.
    *
    * @param constant
    */
    void insertConstant(CONST constant);

    /**
    * Inserts a procedure into the PKB.
    *
    * @param proc
    */
    void insertProcedure(PROC_NAME proc);

    /**
    * Checks whether a PKBField is valid for its given entity type.
    *
    * @param field
    * @return bool
    */
    bool validate(const PKBField field) const;

    /**
    * Checks whether a variable PKBField is valid. If the field is concrete, check if it is in VariableTable.
    *
    * @param field
    * @return bool
    */
    bool validateVariable(const PKBField field) const;

    /**
    * Checks whether a procedure PKBField is valid. If the field is concrete, check if it is in ProcedureTable.
    *
    * @param field
    * @return bool
    */
    bool validateProcedure(const PKBField field) const;

    /**
    * Checks whether a statement PKBField is valid. If the field is concrete, for all the provided fields
    * (statement number, type, or attribute), check if it matches any entry in StatementTable. Append any missing
    * fields.
    *
    * @param field
    * @return bool
    */
    bool validateStatement(const PKBField field) const;

    /**
    * For concrete statement fields, replace its STMT_LO content with the one in the StatementTable. This function will
    * only run after PKB::validate and PKB::validateStatement return true.
    *
    * @param field
    */
    void appendStatementInformation(PKBField* field);
};
