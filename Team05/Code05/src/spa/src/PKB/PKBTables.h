#pragma once

#include <set>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <map>
#include <iterator>
#include <optional>
#include <memory>
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
* A data structure to store program design abstractions. Base class of *RelationshipTables.
*/
class RelationshipTable {
public:
    RelationshipTable(PKBRelationship);

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
    virtual bool contains(PKBField field1, PKBField field2) = 0;

    /**
    * Inserts a RelationshipRow representing Relationship(field1, field2) into the RelationshipTable.
    *
    * @param field1 the first program design entity in the relationship
    * @param field2 the second program design entity in the relationship
    */
    virtual void insert(PKBField field1, PKBField field2) = 0;

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
    virtual FieldRowResponse retrieve(PKBField field1, PKBField field2) = 0;

    /**
    * Retrieves the type of relationships the RelationshipTable stores.
    *
    * @return type of relationship
    */
    PKBRelationship getType();

    virtual int getSize() = 0;

protected:
    PKBRelationship type;

    /**
    * Checks if the two PKBFields provided can be inserted into the table (whether it can exist in the table).
    * Checks that the fields are concrete and have valid STMT_LOs.
    *
    * @param field1 the first PKBField
    * @param field2 the second PKBField
    *
    * @return bool
    * @see PKBField
    */
    virtual bool isInsertOrContainsValid(PKBField field1, PKBField field2) = 0;

    /**
    * Checks if the two PKBFields provided can be retrieved from the table.
    *
    * @param field1 the first PKBField
    * @param field2 the second PKBField
    *
    * @return bool
    * @see PKBField
    */
    virtual bool isRetrieveValid(PKBField field1, PKBField field2) = 0;
};

/**
* A data structure to store program design abstractions as RelationshipRows. Inherits RelationshipTable.
*/
class NonTransitiveRelationshipTable : public RelationshipTable {
public:
    NonTransitiveRelationshipTable(PKBRelationship);

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

    int getSize();

private:
    std::unordered_set<RelationshipRow, RelationshipRowHash> rows;

    /**
    * Checks if the two PKBFields provided can be inserted into the table (whether it can exist in the table).
    * Checks that the fields are concrete and have valid STMT_LOs.
    *
    * @param field1 the first PKBField
    * @param field2 the second PKBField
    *
    * @return bool
    * @see PKBField
    */
    bool isInsertOrContainsValid(PKBField field1, PKBField field2);

    /**
    * Checks if the two PKBFields provided can be retrieved from the table.
    *
    * @param field1 the first PKBField
    * @param field2 the second PKBField
    *
    * @return bool
    * @see PKBField
    */
    bool isRetrieveValid(PKBField field1, PKBField field2);
};

using Result = std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>;

struct Node {
    Node() {};
};

class Graph {
public:
    /**
    * Adds an edge between two STMT_LOs to represent a Follows relationship. Initialise FollowsNodes for
    * the STMT_LOs if they are not present in the graph.
    *
    * @param u the first STMT_LO in a Follows(u,v) relationship
    * @param v the second STMT_LO in a Follows(u,v) relationship
    */
    virtual void addEdge(STMT_LO u, STMT_LO v) = 0;

    /**
    * Checks if Follows(field1, field2) is in the graph.
    *
    * @param field1 the first STMT_LO in a Follows(u,v) query wrapped in a PKBField
    * @param field2 the second STMT_LO in a Follows(u,v) query wrapped in a PKBField
    *
    * @return bool whether Follows(field1, field2) is in the graph
    * @see PKBField
    */
    virtual bool getContains(PKBField field1, PKBField field2) = 0;

    /**
    * Checks if Follows*(field1, field2) is in the graph.
    *
    * @param field1 the first STMT_LO in a Follows*(u,v) query wrapped in a PKBField
    * @param field2 the second STMT_LO in a Follows*(u,v) query wrapped in a PKBField
    *
    * @return bool whether Follows*(field1, field2) is in the graph
    * @see PKBField
    */
    virtual bool getContainsT(PKBField field1, PKBField field2) = 0;

    /**
    * Gets all pairs of PKBFields that satisfy the provided Follows relationship, Follows(field1, field2).
    *
    * @param field1 the first STMT_LO in a Follows(u,v) query wrapped in a PKBField
    * @param field2 the second STMT_LO in a Follows(u,v) query wrapped in a PKBField
    *
    * @return std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> all pairs of PKBFields
    * that satisfy Follows(field1, field2)
    * @see PKBField
    */
    virtual Result retrieve(PKBField field1, PKBField field2) = 0;

    /**
    * Gets all pairs of PKBFields that satisfy the provided Follows* relationship, Follows*(field1, field2).
    *
    * @param field1 the first STMT_LO in a Follows*(u,v) query wrapped in a PKBField
    * @param field2 the second STMT_LO in a Follows*(u,v) query wrapped in a PKBField
    *
    * @return std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> all pairs of PKBFields
    * that satisfy Follows*(field1, field2)
    */
    virtual Result retrieveT(PKBField field1, PKBField field2) = 0;

    virtual int getSize() = 0;
};

/**
* A bi-drectional node inside a FollowsGraph. Inherits Node.
*
* @see FollowsGraph
*/
struct FollowsNode : Node {
    FollowsNode(STMT_LO stmt, FollowsNode* prev, FollowsNode* next) : stmt(stmt), prev(prev), next(next) {}

    STMT_LO stmt;
    FollowsNode* prev; /**< The predecessor of this FollowsNode. */
    FollowsNode* next; /**< The descendant of this FollowsNode. */
};

/**
* A data structure that consists of FollowNodes, where each edge represents a valid Follows relationship.
*
* @see FollowsNode
*/
class FollowsGraph : public Graph {
public:
    /**
    * Adds an edge between two STMT_LOs to represent a Follows relationship. Initialise FollowsNodes for
    * the STMT_LOs if they are not present in the graph.
    *
    * @param u the first STMT_LO in a Follows(u,v) relationship
    * @param v the second STMT_LO in a Follows(u,v) relationship
    */
    void addEdge(STMT_LO u, STMT_LO v);

    /**
    * Checks if Follows(field1, field2) is in the graph.
    *
    * @param field1 the first STMT_LO in a Follows(u,v) query wrapped in a PKBField
    * @param field2 the second STMT_LO in a Follows(u,v) query wrapped in a PKBField
    *
    * @return bool whether Follows(field1, field2) is in the graph
    * @see PKBField
    */
    bool getContains(PKBField field1, PKBField field2);

    /**
    * Checks if Follows*(field1, field2) is in the graph.
    *
    * @param field1 the first STMT_LO in a Follows*(u,v) query wrapped in a PKBField
    * @param field2 the second STMT_LO in a Follows*(u,v) query wrapped in a PKBField
    *
    * @return bool whether Follows*(field1, field2) is in the graph
    * @see PKBField
    */
    bool getContainsT(PKBField field1, PKBField field2);

    /**
    * Gets all pairs of PKBFields that satisfy the provided Follows relationship, Follows(field1, field2).
    *
    * @param field1 the first STMT_LO in a Follows(u,v) query wrapped in a PKBField
    * @param field2 the second STMT_LO in a Follows(u,v) query wrapped in a PKBField
    *
    * @return std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> all pairs of PKBFields
    * that satisfy Follows(field1, field2)
    * @see PKBField
    */
    Result retrieve(PKBField field1, PKBField field2);

    /**
    * Gets all pairs of PKBFields that satisfy the provided Follows* relationship, Follows*(field1, field2).
    *
    * @param field1 the first STMT_LO in a Follows*(u,v) query wrapped in a PKBField
    * @param field2 the second STMT_LO in a Follows*(u,v) query wrapped in a PKBField
    *
    * @return std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> all pairs of PKBFields
    * that satisfy Follows*(field1, field2)
    */
    Result retrieveT(PKBField field1, PKBField field2);

    int getSize();

private:
    std::map<STMT_LO, FollowsNode*> nodes; /**< The list of nodes in this FollowsGraph */

    /**
    * Gets all pairs of PKBFields that satisfy the provided Follows* relationship, Follows*(field1, field2),
    * where field1 is a concrete field and field2 is either a statement declaration or a statement wildcard.
    * Statement wildcards are treated as a statement declaration for any statement type.
    *
    * @param field1 a concrete field to begin the traversal from
    * @param field2 a statement declaration
    *
    * @return std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> all pairs of PKBFields where the
    * second item in each pair satisfy the statement declaration.
    * @see PKBField
    */
    Result traverseStartT(PKBField field1, PKBField field2);

    /**
    * An overloaded helper function that traverses the graph forward starting at the provided node
    * until there is no next node.
    *
    * @param stmtSetPtr a pointer to a set of STMT_LO that stores the possible STMT_LO for the second field in a
    * Follows* relationship.
    * @param node a pointer to the node to begin traversal from
    * @see PKBField
    */
    void traverseStartT(std::set<STMT_LO>* stmtSetPtr, FollowsNode* node);

    /**
    * Gets all pairs of PKBFields that satisfy the provided Follows* relationship, Follows*(field1, field2),
    * where field1 is either a statement declaration or a statement wildcard and field2 is a concrete field.
    * Statement wildcards are treated as a statement declaration for any statement type.
    *
    * @param field1 a concrete field to begin the traversal from
    * @param field2 a statement declaration
    *
    * @return std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> all pairs of PKBFields where the
    * second item in each pair satisfy the statement declaration.
    * @see PKBField
    */
    Result traverseEndT(PKBField field1, PKBField field2);

    /**
    * An overloaded helper function that traverses the graph backwards starting at the provided node
    * until there is no next node.
    *
    * @param stmtSetPtr a pointer to a set of STMT_LO that stores the possible STMT_LO for the first field in a
    * Follows* relationship.
    * @param node a pointer to the node to begin traversal from
    * @see PKBField
    */
    void traverseEndT(std::set<STMT_LO>* stmtSetPtr, FollowsNode* node);

    /**
    * Gets all pairs of PKBFields that satisfy the provided Follows relationship, Follows(field1, field2),
    * where both field1 and field2 are either statement declarations or statement wildcards.
    * Statement wildcards are treated as a statement declaration for any statement type.
    *
    * Internally, iterates through the nodes in the graph calls traverseStart with each node.
    *
    * @param field1 the first statement declaration
    * @param field2 the second statement declaration
    *
    * @return std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> all pairs of PKBFields where the
    * second item in each pair satisfy the statement declarations.
    * @see PKBField
    */
    Result traverseAll(StatementType type1, StatementType type2);

    /**
    * Gets all pairs of PKBFields that satisfy the provided Follows* relationship, Follows*(field1, field2),
    * where both field1 and field2 are either statement declarations or statement wildcards.
    * Statement wildcards are treated as a statement declaration for any statement type.
    *
    * Internally, iterates through the nodes in the graph calls traverseStart with each node.
    *
    * @param field1 the first statement declaration
    * @param field2 the second statement declaration
    *
    * @return std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> all pairs of PKBFields where the
    * second item in each pair satisfy the statement declarations.
    * @see PKBField
    */
    Result traverseAllT(StatementType type1, StatementType type2);
};


/**
* A bi-directional node inside a ParentGraph
*
* @see ParentGraph
*/
struct ParentNode : Node {
    using NodeSet = std::vector<ParentNode*>;
    ParentNode(STMT_LO stmt, ParentNode* prev, NodeSet next) : stmt(stmt), next(next), prev(prev) {}

    STMT_LO stmt;
    NodeSet next; /**< The descendants of this ParentNode. */
    ParentNode* prev; /**< The predecessor of this ParentNode. */
};

/**
* A data structure that consists of ParentNodes, where each edge represents a valid Parent relationship.
*
* @see ParentNode
*/
class ParentGraph : public Graph {
public:
    /**
    * Adds an edge between two STMT_LOs to represent a Parent relationship. Initialises ParentNodes for
    * the STMT_LOs if they are not present in the graph.
    *
    * @param u the first STMT_LO in a Parent(u,v) relationship
    * @param v the second STMT_LO in a Parent(u,v) relationship
    */
    void addEdge(STMT_LO u, STMT_LO v);

    /**
    * Checks if Parent(field1, field2) is in the graph.
    *
    * @param field1 the first STMT_LO in a Parent(u,v) query wrapped in a PKBField
    * @param field2 the second STMT_LO in a Parent(u,v) query wrapped in a PKBField
    *
    * @return bool true if Parent(field1, field2) is in the graph and false otherwise
    * @see PKBField
    */
    bool getContains(PKBField field1, PKBField field2);

    /**
    * Checks if Parent*(field1, field2) is in the graph.
    *
    * @param field1 the first STMT_LO in a Parent*(u,v) query wrapped in a PKBField
    * @param field2 the second STMT_LO in a Parent*(u,v) query wrapped in a PKBField
    *
    * @return bool true if Parent*(field1, field2) is in the graph and false otherwise
    * @see PKBField
    */
    bool getContainsT(PKBField field1, PKBField field2);


    /**
    * Gets all pairs of PKBFields that satisfy the provided Parent relationship, Parent(field1, field2).
    *
    * @param field1 the first STMT_LO in a Parent(u,v) query wrapped in a PKBField
    * @param field2 the second STMT_LO in a Parent(u,v) query wrapped in a PKBField
    *
    * @return std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> all pairs of PKBFields
    *   that satisfy Parent(field1, field2)
    * @see PKBField
    */
    Result retrieve(PKBField field1, PKBField field2);

    /**
    * Gets all pairs of PKBFields that satisfy the provided Parent* relationship, Parent*(field1, field2).
    *
    * @param field1 the first STMT_LO in a Parent*(u,v) query wrapped in a PKBField
    * @param field2 the second STMT_LO in a Parent*(u,v) query wrapped in a PKBField
    *
    * @return std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> all pairs of PKBFields
    *   that satisfy Parent*(field1, field2)
    */
    Result retrieveT(PKBField field1, PKBField field2);

    int getSize();

private:
    std::map<STMT_LO, ParentNode*> nodes; /**< The list of nodes in this FollowsGraph */

    /**
    * Gets all pairs of PKBFields that satisfy the provided Parent* relationship, Parent*(field1, field2),
    * where field1 is a concrete field and field2 is either a statement declaration or a statement wildcard.
    * Statement wildcards are treated as a statement declaration for any statement type.
    *
    * @param field1 a concrete field to begin the traversal from
    * @param field2 a statement declaration
    *
    * @return std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> all pairs of PKBFields where the
    *   second item in each pair satisfies the statement declaration.
    *
    * @see PKBField
    */
    Result traverseStartT(PKBField field1, PKBField field2);

    /**
    * An overloaded helper function that traverses the graph forward starting at the provided node
    * until there are no next nodes left.
    *
    * @param found a pointer to a set of STMT_LO that stores the possible STMT_LO for the second field in a
    *   Parent* relationship.
    * @param node a pointer to the node to begin traversal from
    * @param targetType the desired statement type for STMT_LOs that are encountered during the traversal
    *
    * @see PKBField
    */
    void traverseStartT(std::set<STMT_LO>* found, ParentNode* node, StatementType targetType);

    /**
    * Gets all pairs of PKBFields that satisfy the provided Parent* relationship, Parent*(field1, field2),
    * where field1 is either a statement declaration or a statement wildcard and field2 is a concrete field.
    * Statement wildcards are treated as a statement declaration for any statement type.
    *
    * @param field1 a concrete field to begin the traversal from
    * @param field2 a statement declaration
    *
    * @return std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> all pairs of PKBFields where the
    * second item in each pair satisfies the statement declaration.
    *
    * @see PKBField
    */
    Result traverseEndT(PKBField field1, PKBField field2);

    /**
    * An overloaded helper function that traverses the graph backwards starting at the provided node
    * until there are no prev nodes left.
    *
    * @param found a pointer to a set of STMT_LO that stores the possible STMT_LO for the first field in a
    *   Parent* relationship.
    * @param node a pointer to the node to begin traversal from
    * @param targetType the desired statement type for STMT_LOs that are encountered during the traversal
    *
    * @see PKBField
    */
    void traverseEndT(std::set<STMT_LO>* found, ParentNode* node, StatementType targetType);

    /**
    * Gets all pairs (field1, field2) of PKBFields that satisfy the provided Parent* relationship,
    * Parent*(field1, field2), where field1 is a statement of type1 and field2 a statement of type2.
    *
    * Internally, iterates through the nodes in the graph and calls traverseStart on each node.
    *
    * @param type1 the first statement type
    * @param type2 the second statement type
    *
    * @return std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> all pairs of PKBFields where each
    *   item in each pair satisfies the corresponding statement type requirement.
    *
    * @see PKBField
    */
    Result traverseAllT(StatementType type1, StatementType type2);

    /**
    * Gets all pairs (field1, field2) of PKBFields that satisfy the provided Parent relationship,
    * Parent(field1, field2), where field1 is a statement of type1 and field2 is a statement of type2.
    *
    * Internally, iterates through the nodes in the graph calls traverseStart with each node.
    *
    * @param field1 the first statement type
    * @param field2 the second statement type
    *
    * @return std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> all pairs of PKBFields where
    * each item in each pair satisfies the corresponding statement type requirement.
    * @see PKBField
    */
    Result traverseAll(StatementType type1, StatementType type2);
};

class TransitiveRelationshipTable : public RelationshipTable {
public:
    explicit TransitiveRelationshipTable(PKBRelationship);
    
    /**
    * Gets all pairs of PKBFields that satisfy the provided RELATIONSHIP*(field1, field2).
    * where RELATIONSHIP = Follows || Parents
    *
    * @param field1 the first program design entity in the relationship
    * @param field2 the first program design entity in the relationship
    *
    * @return std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> all pairs of PKBFields
    * that satisfy RELATIONSHIP*(field1, field2)
    * @see PKBField
    */
    virtual FieldRowResponse retrieveT(PKBField field1, PKBField field2) = 0;

    virtual int getSize() = 0;
};

/**
* A data structure to store Modifies program design abstractions as RelationshipRows. Inherits RelationshipTable.
*/
class ModifiesRelationshipTable : public NonTransitiveRelationshipTable {
public:
    ModifiesRelationshipTable();
};

/**
* A data structure to store Uses program design abstractions as RelationshipRows. Inherits RelationshipTable.
*/
class UsesRelationshipTable : public NonTransitiveRelationshipTable {
public:
    UsesRelationshipTable();
};

/**
 * A data structure to store Follows and FollowsT program design abstractions as FollowsNodes in a FollowsGraph.
 * Inherits from TransitiveRelationshipTable.
 *
 * @see FollowsNode, FollowsGraph, TransitiveRelationshipTable
 */
class FollowsRelationshipTable : public TransitiveRelationshipTable {
public:
    FollowsRelationshipTable();

    /**
    * Inserts into FollowsGraph an edge representing Follows(field1, field2).
    * If the two provided entities are not valid statements or are not concrete, no insert will be done
    * and an error will be thrown.
    *
    * @param field1 the first STMT_LO in a Follows(u,v) wrapped in a PKBField
    * @param field2 the second STMT_LO in a Follows(u,v) wrapped in a PKBField
    * @see PKBField
    */
    void insert(PKBField field1, PKBField field2);

    /**
    * Checks whether the FollowsRelationshipTable contains Follows(field1, field2).
    *
    * @param field1 the first STMT_LO in a Follows(u,v) query wrapped in a PKBField
    * @param field2 the second STMT_LO in a Follows(u,v) query wrapped in a PKBField
    *
    * @return bool whether Follows(field1, field2) is in the graph
    * @see PKBField
    */
    bool contains(PKBField field1, PKBField field2);

    /**
    * Checks whether the FollowsRelationshipTable contains Follows*(field1, field2).
    *
    * @param field1 the first STMT_LO in a Follows*(u,v) query wrapped in a PKBField
    * @param field2 the second STMT_LO in a Follows*(u,v) query wrapped in a PKBField
    *
    * @return bool whether Follows(field1, field2) is in the graph
    * @see PKBField
    */
    bool containsT(PKBField field1, PKBField field2);

    /**
    * Retrieves all pairs of statements that satisfies Follows(field1, field2).
    *
    * @param field1 the first STMT_LO in a Follows(u,v) query wrapped in a PKBField
    * @param field2 the second STMT_LO in a Follows(u,v) query wrapped in a PKBField
    *
    * @return std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> an unordered set of vectors of PKBFields,
    * where each vector represents the two program design entities in a Follows relationship,
    * i.e. Follows(field1, field2) -> [field1, field2].
    * @see PKBField
    */
    FieldRowResponse retrieve(PKBField field1, PKBField field2);

    /**
    * Retrieves all pairs of statements that satisfies Follows*(field1, field2).
    *
    * @param field1 the first STMT_LO in a Follows*(u,v) query wrapped in a PKBField
    * @param field2 the second STMT_LO in a Follows*(u,v) query wrapped in a PKBField
    *
    * @return std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> an unordered set of vectors of PKBFields,
    * where each vector represents the two program design entities in a Follows relationship,
    * i.e. Follows*(field1, field2) -> [field1, field2].
    * @see PKBField
    */
    FieldRowResponse retrieveT(PKBField field1, PKBField field2);

    int getSize();

private:
    std::unique_ptr<FollowsGraph> graph;

    /**
    * Checks if the two PKBFields provided can be inserted into the table (whether it can exist in the table).
    * Checks that the fields are concrete and have valid STMT_LOs.
    *
    * @param field1 the first PKBField
    * @param field2 the second PKBField
    *
    * @return bool
    * @see PKBField
    */
    bool isInsertOrContainsValid(PKBField field1, PKBField field2);

    /**
    * Checks if the two PKBFields provided can be retrieved from the table.
    *
    * @param field1 the first PKBField
    * @param field2 the second PKBField
    *
    * @return bool
    * @see PKBField
    */
    bool isRetrieveValid(PKBField field1, PKBField field2);
};

/**
* A data structure to store Parent and Parent* program design abstractions as ParentNodes in a ParentGraph.
* Inherits from TransitiveRelationshipTable
*
* @see ParentNode, ParentGraph, TransitiveRelationshipTable
*/
class ParentRelationshipTable : public TransitiveRelationshipTable {
public:
    ParentRelationshipTable();

    /**
    * Checks whether the FollowsRelationshipTable contains Parent(field1, field2).
    *
    * @param field1 the first STMT_LO in a Parent(u,v) query wrapped in a PKBField
    * @param field2 the second STMT_LO in a Parent(u,v) query wrapped in a PKBField
    *
    * @return bool whether Parent(field1, field2) is in the graph
    *
    * @see PKBField
    */
    bool contains(PKBField field1, PKBField field2);

    /**
    * Inserts into ParentGraph an edge representing Parent(field1, field2).
    * If the two provided entities are not valid statements or are not concrete, no insert will be done
    * and an error will be thrown.
    *
    * @param field1 the first STMT_LO in a Parent(u,v) relationship wrapped in a PKBField
    * @param field2 the second STMT_LO in a Parent(u,v) relationship wrapped in a PKBField
    *
    * @see PKBField
    */
    void insert(PKBField field1, PKBField field2);

    /**
    * Retrieves all pairs of statements that satisfies Parent(field1, field2).
    *
    * @param field1 the first STMT_LO in a Parent(u,v) query wrapped in a PKBField
    * @param field2 the second STMT_LO in a Parent(u,v) query wrapped in a PKBField
    *
    * @return std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> an unordered set of vectors of PKBFields,
    *   where each vector represents the two program design entities in a Parent relationship,
    *   i.e. Parent(field1, field2) -> [field1, field2].
    *
    * @see PKBField
    */
    FieldRowResponse retrieve(PKBField field1, PKBField field2);

    /**
    * Checks whether the ParentRelationshipTable contains Parent*(field1, field2).
    *
    * @param field1 the first STMT_LO in a Parent*(u,v) query wrapped in a PKBField
    * @param field2 the second STMT_LO in a Parent*(u,v) query wrapped in a PKBField
    *
    * @return bool whether Parent*(field1, field2) is in the graph
    *
    * @see PKBField
    */
    bool containsT(PKBField field1, PKBField field2);

    /**
    * Retrieves all pairs of statements that satisfies Parent*(field1, field2).
    *
    * @param field1 the first STMT_LO in a Parent*(u,v) query wrapped in a PKBField
    * @param field2 the second STMT_LO in a Parent*(u,v) query wrapped in a PKBField
    *
    * @return std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> an unordered set of vectors of PKBFields,
    *   where each vector represents the two program design entities in a Parent* relationship,
    *   i.e. Parent*(field1, field2) -> [field1, field2].
    *
    * @see PKBField
    */
    FieldRowResponse retrieveT(PKBField field1, PKBField field2);

    int getSize();

private:
    std::unique_ptr<ParentGraph> graph;

    /**
    * Checks if the two PKBFields provided can be inserted into the table (whether it can exist in the table).
    * Checks that the fields are concrete and have valid STMT_LOs.
    *
    * @param field1 the first PKBField
    * @param field2 the second PKBField
    *
    * @return bool
    *
    * @see PKBField
    */
    bool isInsertOrContainsValid(PKBField, PKBField);

    /**
    * Checks if the two PKBFields provided can be retrieved from the table.
    *
    * @param field1 the first PKBField
    * @param field2 the second PKBField
    *
    * @return bool
    *
    * @see PKBField
    */
    bool isRetrieveValid(PKBField field1, PKBField field2);
};
