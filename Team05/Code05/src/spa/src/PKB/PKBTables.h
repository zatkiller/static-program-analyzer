#pragma once

#include <set>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <iterator>
#include <optional>
#include "PKBDataTypes.h"
#include "PKBField.h"
#include "PKBRelationship.h"

/**
* A data structure to store constants (integers) in a ConstantTable.
*/
class ConstantRow {
public:
    explicit ConstantRow(CONST value) : constant(value) {}

    /**
    * Retrieves the constant stored in the ConstantRow.
    *
    * @return a constant wrapped in CONST
    */
    CONST getConst() const;
    bool operator == (const ConstantRow&) const;
    bool operator < (const ConstantRow& other) const;

private:
    CONST constant;
};

/**
* Hash function for ConstantRow.
*/
class ConstantRowHash {
public:
    size_t operator() (const ConstantRow&) const;
};

/**
* A data structure to store ConstantRows.
*
* @see ConstantRow
*/
class ConstantTable {
public:
    /**
    * Checks whether the ConstantTable contains the constant wrapped in a PKBField.
    *
    * @param const the constant to be checked
    * @return bool whether constant is present in the ConstantTable
    * @see CONST
    */
    bool contains(CONST constant) const;

    /**
    * Inserts a constant wrapped in a CONST into the ConstantTable.
    *
    * @param entry the constant to be inserted
    * @see CONST
    */
    void insert(CONST constant);

    /**
    * Returns the number of ConstantRows in the ConstantTable.
    *
    * @return int number of ConstantRows in the ConstantTable
    */
    int getSize() const;

    /**
    * Retrieves a vector of all constants stored in the ConstantTable.
    *
    * @return std::vector<CONST> a vector of constants
    * @see CONST
    */
    std::vector<CONST> getAllConst() const;

private:
    std::set<ConstantRow> rows;
};

/**
* A data structure to store procedure names (strings) in a ProcedureTable.
*/
class ProcedureRow {
public:
    explicit ProcedureRow(PROC_NAME);

    /**
    * Retrieves the procedure name stored in the ProcedureRow.
    *
    * @return a procedure name wrapped in PROC_NAME
    */
    PROC_NAME getProcName() const;
    bool operator == (const ProcedureRow&) const;
    bool operator < (const ProcedureRow& other) const;

private:
    PROC_NAME procedureName;
};

/**
* Hash function for ProcedureRow.
*/
class ProcedureRowHash {
public:
    size_t operator() (const ProcedureRow&) const;
};

/**
* A data structure to store ProcedureRows.
* 
* @see ProcedureRow
*/
class ProcedureTable {
public:
    /**
    * Checks whether the ProcedureTable contains the procedure name.
    *
    * @param name
    * @returns whether the procedure name is present in the ProcedureTable
    */
    bool contains(std::string name);

    /**
    * Inserts a procedure name into the ProcedureTable.
    *
    * @param name
    */
    void insert(std::string name);

    /**
    * Returns the number of ProcedureRows.
    *
    * @return number of ProcedureRows
    */
    int getSize();

    /**
    * Retrieves all procedure names stored in the ProcedureTable.
    *
    * @return a vector of procedure names
    */
    std::vector<PROC_NAME> getAllProcs();

private:
    std::set<ProcedureRow> rows;
};

/**
* A data structure to store statement information (line number and statement type) in a StatementTable.
*/
class StatementRow {
public:
    StatementRow(StatementType, int);

    /**
    * Retrieves the statement information stored in the StatementRow.
    *
    * @return the statement information wrapped in CONST
    */
    STMT_LO getStmt() const;
    bool operator == (const StatementRow&) const;
    bool operator < (const StatementRow& other) const;

private:
    STMT_LO stmt;
};

/**
* Hash function for StatementRow.
*/
class StatementRowHash {
public:
    size_t operator() (const StatementRow& other) const;
};


/**
* A data structure to store StatementRows.
*/
class StatementTable {
public:
    /**
    * Checks whether the StatementTable contains the statement information wrapped in a PKBField.
    *
    * @param entry the PKBField containing the statement information to be checked
    * @returns whether the statement information is present in the ConstantTable
    */
    bool contains(StatementType, int);

    bool contains(int);

    /**
    * Inserts statement information wrapped in a PKBField into the StatementTable.
    *
    * @param entry the PKBField containing the statement information to be inserted
    */
    void insert(StatementType, int);

    /**
    * Returns the number of StatementRows.
    *
    * @return number of StatementRows
    */
    int getSize();

    /**
    * Retrieves all statement information stored in the StatementTable.
    *
    * @return a vector of statement information
    */
    std::vector<STMT_LO> getAllStmt();

    /**
    * Retrieves all statement information of the given type stored in the StatementTable.
    *
    * @param type type of statement
    * @return a vector of statement information belong to the given type
    */
    std::vector<STMT_LO> getStmtOfType(StatementType);

    /**
    * Retrieve the statement type of a statement row with statementNum. If there doesn't exist one,
    * return a null optional.
    *
    * @param statementNum
    * @return std::optional<StatementType>
    */
    std::optional<StatementType> getStmtTypeOfLine(int statementNum);

private:
    std::set<StatementRow> rows;
};

/**
* A data structure to store variable names (strings) in a VariableTable.
*/
class VariableRow {
public:
    explicit VariableRow(VAR_NAME);

    /**
    * Retrieves the variable name stored in the VariableTable.
    *
    * @return a variable name wrapped in VAR_NAME
    */
    VAR_NAME getVarName() const;
    bool operator == (const VariableRow&) const;
    bool operator < (const VariableRow& other) const;

private:
    VAR_NAME variableName;
};

/**
* Hash function for VariableTable.
*/
class VariableRowHash {
public:
    size_t operator()(const VariableRow&) const;
};

/**
* A data structure to store VariableRows.
*/
class VariableTable {
public:
    /**
    * Checks whether the VariableTable contains the variable name wrapped in a PKBField.
    *
    * @param name the PKBField containing the variable name to be checked
    * @returns whether the variable name is present in the VariableTable
    */
    bool contains(std::string name);

    /**
    * Inserts a variable name wrapped in a PKBField into the VariableTable.
    *
    * @param name the PKBField containing the variable name to be inserted
    */
    void insert(std::string name);

    /**
    * Returns the number of VariableRows.
    *
    * @return number of VariableRows
    */
    int getSize();

    /**
    * Retrieves all variable names stored in the VariableTable.
    *
    * @return a vector of variable names
    */
    std::vector<VAR_NAME> getAllVars();

private:
    std::set<VariableRow> rows;
};

/**
* A data structure to store a program design abstraction (relationship) which has two fields.
*/
class RelationshipRow {
public:
    RelationshipRow(PKBField, PKBField);

    /**
    * Retrieves the first field in the relationship.
    */
    PKBField getFirst() const;

    /**
    * Retrieves the second field in the relationship.
    */
    PKBField getSecond() const;

    bool operator == (const RelationshipRow&) const;

private:
    PKBField field1;
    PKBField field2;
};

/**
* Hash function for RelationshipRow.
*/
class RelationshipRowHash {
public:
    size_t operator() (const RelationshipRow&) const;
};

using FieldRowResponse = std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>;

/**
* A data structure to store program design abstractions as RelationshipRows. Base class of *RelationshipTables.
*/
class RelationshipTable {
public:
    explicit RelationshipTable(PKBRelationship);

    /**
    * Checks whether the RelationshipTable contains a RelationshipRow representing
    * Relationship(field1, field2). If either fields are not concrete or if a statement field
    * has no corresponding row in statement table, return false.
    *
    * @param field1 the first program design entity in the relationship
    * @param field2 the second program design entity in the relationship
    *
    * @return whether the relationship is present in the RelationshipTable
    */
    bool contains(PKBField field1, PKBField field2);

    /**
    * Inserts a RelationshipRow representing Relationship(field1, field2) into the RelationshipTable.
    *
    * @param field1 the first program design entity in the relationship
    * @param field2 the second program design entity in the relationship
    */
    void insert(PKBField field1, PKBField field2);

    /**
    * Retrieves all pairs of PKBFields in table that satisfies the parameters. If a statement field
    * has no corresponding row in statement table, or if the first parameter is a wildcard, return false.
    *
    * @param field1 the first program design entity in the query
    * @param field2 the second program design entity in the query
    * @return FieldRowResponse
    *
    * @see PKBField
    */
    FieldRowResponse retrieve(PKBField field1, PKBField field2);

    bool isInsertOrContainsValid(PKBField field1, PKBField field2);
    bool isRetrieveValid(PKBField field1, PKBField field2);

    /**
    * Retrieves the type of relationships the RelationshipTable stores.
    *
    * @return type of relationship
    */
    PKBRelationship getType();

    int getSize();

protected:
    std::unordered_set<RelationshipRow, RelationshipRowHash> rows;
    PKBRelationship type;
};

/**
* A data structure to store Modifies program design abstractions as RelationshipRows. Inherits RelationshipTable.
*/
class ModifiesRelationshipTable : public RelationshipTable {
public:
    ModifiesRelationshipTable();
};

/**
* A data structure to store Uses program design abstractions as RelationshipRows. Inherits RelationshipTable.
*/
class UsesRelationshipTable : public RelationshipTable {
public:
    UsesRelationshipTable();
};
