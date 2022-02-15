#pragma once

#include <map>
#include <vector>
#include <set>
#include <unordered_set>
#include <stack>
#include "PKBField.h"
#include <memory>

using Result = std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>;

/**
* A bi-drectional node inside a FollowsGraph.
* 
* @see FollowsGraph
*/
struct FollowsNode {
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
class FollowsGraph {
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
    * @return bool whether Follows(field1, field2) is in the graph
    * 
    * @see PKBField
    */
    bool getContains(PKBField field1, PKBField field2);

    /**
    * Checks if Follows*(field1, field2) is in the graph.
    *
    * @param field1 the first STMT_LO in a Follows*(u,v) query wrapped in a PKBField
    * @param field2 the second STMT_LO in a Follows*(u,v) query wrapped in a PKBField
    * @return bool whether Follows*(field1, field2) is in the graph
    * 
    * @see PKBField
    */
    bool getContainsT(PKBField field1, PKBField field2);

    /**
    * Gets all pairs of PKBFields that satisfy the provided Follows relationship, Follows(field1, field2).
    *
    * @param field1 the first STMT_LO in a Follows(u,v) query wrapped in a PKBField
    * @param field2 the second STMT_LO in a Follows(u,v) query wrapped in a PKBField
    * @return std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> all pairs of PKBFields
    * that satisfy Follows(field1, field2)
    * 
    * @see PKBField
    */
    Result getFollows(PKBField field1, PKBField field2);

    /**
    * Gets all pairs of PKBFields that satisfy the provided Follows* relationship, Follows*(field1, field2).
    *
    * @param field1 the first STMT_LO in a Follows*(u,v) query wrapped in a PKBField
    * @param field2 the second STMT_LO in a Follows*(u,v) query wrapped in a PKBField
    * @return std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> all pairs of PKBFields
    * that satisfy Follows*(field1, field2)
    */
    Result getFollowsT(PKBField field1, PKBField field2);

private:
    std::map<STMT_LO, FollowsNode*> nodes; /**< The list of nodes in this FollowsGraph */

    /**
    * Gets all pairs of PKBFields that satisfy the provided Follows* relationship, Follows*(field1, field2),
    * where field1 is a concrete field and field2 is either a statement declaration or a statement wildcard.
    * Statement wildcards are treated as a statement declaration for any statement type.
    * 
    * @param field1 a concrete field to begin the traversal from
    * @param field2 a statement declaration
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
    * @param field1 a statement declaration
    * @param field2 a statement declaration
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
    * @param field1 a statement declaration
    * @param field2 a statement declaration
    * @return std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> all pairs of PKBFields where the
    * second item in each pair satisfy the statement declarations.
    * @see PKBField
    */
    Result traverseAllT(StatementType type1, StatementType type2);
};
