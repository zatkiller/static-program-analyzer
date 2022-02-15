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
* A Node inside a FollowsGraph which is bidirectional
*/
struct FollowsNode {
    FollowsNode(STMT_LO stmt, FollowsNode* prev, FollowsNode* next) : stmt(stmt), prev(prev), next(next) {}

    STMT_LO stmt;
    FollowsNode* prev;
    FollowsNode* next;
};

/**
* A data structure that supports the lookup of transitive Follows* relationships
*/
class FollowsGraph {
public:
    /**
    * Adds an edge between two STMT_LOs to represent a Follows relationship.
    * 
    * @param u the first STMT_LO in the Follows(u,v) relationship
    * @param v the second STMT_LO in the Follows(u,v) relationship
    */
    void addEdge(STMT_LO u, STMT_LO v);
    // wildcard should be converted to Statement::All

    /**
    * Gets all pairs of PKBFields that satisfy the provided Follows* relationship.
    * 
    * @param field1 the first field in the Follows* query
    * @param field2 the second field in the Follows* query
    * @return a Result encapsulating all pairs of PKBFields 
    *   that follow the provided relationship
    */
    Result getFollowsT(PKBField field1, PKBField field2);

    /**
    * Gets all pairs of PKBFields that satisfy the provided Follows relationship.
    *
    * @param field1 the first field in the Follows query
    * @param field2 the second field in the Follows query
    * @return a Result encapsulating all pairs of PKBFields
    *   that follow the provided relationship
    */
    Result getFollows(PKBField field1, PKBField field2);

    /**
    * Checks if the relationship Follows*(field1, field2) is true.
    * 
    * @param field1 the first field in the Follows* query
    * @param field2 the second field in the Follows* query
    * @return true if Follows*(field1, field2) is true and false otherwise
    */
    bool getContainsT(PKBField field1, PKBField field2);

    /**
    * Checks if the relationship Follows(field1, field2) is true.
    *
    * @param field1 the first field in the Follows query
    * @param field2 the second field in the Follows query
    * @return true if Follows(field1, field2) is true and false otherwise
    */
    bool getContains(PKBField field1, PKBField field2);

private:
    std::map<STMT_LO, FollowsNode*> nodes;
    Result traverseStartT(PKBField field1, PKBField field2);
    Result traverseEndT(PKBField field1, PKBField field2);
    Result traverseAllT(StatementType type1, StatementType type2);
    void traverseStartT(std::set<STMT_LO>* stmtSetPtr, StatementType type, FollowsNode* node);
    void traverseEndT(std::set<STMT_LO>* stmtSetPtr, StatementType type, FollowsNode* node);

    Result traverseAll(StatementType type1, StatementType type2);
};
