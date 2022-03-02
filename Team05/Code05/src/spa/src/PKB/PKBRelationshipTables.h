#pragma once

#include <set>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <map>
#include <iterator>
#include <optional>
#include <memory>
#include "logging.h"
#include "PKBField.h"
#include "PKBRelationship.h"

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
    explicit NonTransitiveRelationshipTable(PKBRelationship);

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

/**
* A bi-directional node inside a ParentGraph
*
* @see ParentGraph
*/
struct Node {
    using NodeSet = std::vector<Node*>;
    Node(STMT_LO stmt, Node* prev, NodeSet next) : stmt(stmt), next(next), prev(prev) {}

    STMT_LO stmt;
    NodeSet next; /**< The descendants of this ParentNode. */
    Node* prev; /**< The predecessor of this ParentNode. */
};

/**
* A data structure that consists of ParentNodes, where each edge represents a valid Parent relationship.
*
* @see ParentNode
*/
class Graph {
public:
    explicit Graph(PKBRelationship);

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
    bool contains(PKBField field1, PKBField field2);

    /**
    * Checks if Parent*(field1, field2) is in the graph.
    *
    * @param field1 the first STMT_LO in a Parent*(u,v) query wrapped in a PKBField
    * @param field2 the second STMT_LO in a Parent*(u,v) query wrapped in a PKBField
    *
    * @return bool true if Parent*(field1, field2) is in the graph and false otherwise
    * @see PKBField
    */
    bool containsT(PKBField field1, PKBField field2);


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
    PKBRelationship type;
    std::map<STMT_LO, Node*> nodes; /**< The list of nodes in this FollowsGraph */
    
    Node* addNode(std::map<STMT_LO, Node*>&, STMT_LO);
    
    Result traverseStart(PKBField field1, PKBField field2);

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
    void traverseStartT(std::set<STMT_LO>* found, Node* node, StatementType targetType);

    Result traverseEnd(PKBField field1, PKBField field2);

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
    void traverseEndT(std::set<STMT_LO>* found, Node* node, StatementType targetType);

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
    std::unique_ptr<Graph> graph;

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
};
