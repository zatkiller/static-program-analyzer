#pragma once

#include <map>
#include <vector>
#include <set>
#include <unordered_set>
#include <stack>
#include "PKBField.h"

using Result = std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>;

struct FollowsNode {
    FollowsNode(STMT_LO stmt, FollowsNode* prev, FollowsNode* next) : stmt(stmt), prev(prev), next(next) {};

    STMT_LO stmt;
    FollowsNode* prev;
    FollowsNode* next;
};

class FollowsGraph {
public:
    void addEdge(STMT_LO u, STMT_LO v);
    // wildcard should be converted to Statement::All
    Result getFollowsT(PKBField field1, PKBField field2);

private:
    std::map<STMT_LO, FollowsNode*> nodes;
    Result traverseStart(PKBField field1, PKBField field2);
    //Result traverseEnd(PKBField field1, PKBField field2);
    //Result traverseAll(StatementType type1, StatementType type2);
    void traverseStart(std::set<STMT_LO>* stmtSetPtr, StatementType type, FollowsNode* node);
    //void traverseEnd(std::set<STMT_LO>* stmtSetPtr, StatementType type, STMT_LO stmt);
};
