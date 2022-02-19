#pragma once

#include <map>
#include <vector>
#include <set>
#include <algorithm>
#include <unordered_set>
#include <iterator>

#include "PKBField.h"

using Result = std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>;

/**
* A bi-directional node inside a ParentGraph
* 
* @see ParentGraph
*/
struct ParentNode {
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
class ParentGraph {
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
    Result getParent(PKBField field1, PKBField field2);

    /**
    * Gets all pairs of PKBFields that satisfy the provided Parent* relationship, Parent*(field1, field2).
    *
    * @param field1 the first STMT_LO in a Parent*(u,v) query wrapped in a PKBField
    * @param field2 the second STMT_LO in a Parent*(u,v) query wrapped in a PKBField
    *
    * @return std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> all pairs of PKBFields
    *   that satisfy Parent*(field1, field2)
    */
    Result getParentT(PKBField field1, PKBField field2);

private:
    std::map<STMT_LO, ParentNode*> nodes;

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
