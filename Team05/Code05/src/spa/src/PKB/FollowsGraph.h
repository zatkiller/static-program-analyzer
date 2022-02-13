#pragma once

#include <map>
#include <vector>
#include <set>
#include <unordered_set>
#include <stack>
#include "PKBField.h"

using FollowsTResult = std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>;

class FollowsGraph {
public:
    void addEdge(STMT_LO u, STMT_LO v);
    // wildcard should be converted to Statement::All
    FollowsTResult getFollowsT(PKBField field1, PKBField field2);


private:
    std::map<STMT_LO, STMT_LO> edges;
    std::map<STMT_LO, STMT_LO> reversedEdges;
    FollowsTResult traverseStart(PKBField field1, PKBField field2);
    FollowsTResult traverseEnd(PKBField field1, PKBField field2);
    FollowsTResult traverseAll(StatementType type1, StatementType type2);
    void traverseStart(std::set<STMT_LO>* stmtSetPtr, StatementType type, STMT_LO stmt);
    void traverseEnd(std::set<STMT_LO>* stmtSetPtr, StatementType type, STMT_LO stmt);
};
