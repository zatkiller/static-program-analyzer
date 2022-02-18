#pragma once

#include <map>
#include <vector>
#include <set>
#include <algorithm>
#include <unordered_set>
#include <iterator>

#include "PKBField.h"

using Result = std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>;

struct ParentNode {
    using NodeSet = std::vector<ParentNode*>;
    ParentNode(STMT_LO stmt, ParentNode* prev, NodeSet next) : stmt(stmt), next(next), prev(prev) {}

    STMT_LO stmt;
    NodeSet next;
    ParentNode *prev;
};

class ParentGraph {
public:
    void addEdge(STMT_LO u, STMT_LO v);
    bool getContains(PKBField field1, PKBField field2);
    bool getContainsT(PKBField field1, PKBField field2);
    Result getParent(PKBField field1, PKBField field2);
    Result getParentT(PKBField field1, PKBField field2);

private:
    std::map<STMT_LO, ParentNode*> nodes;
    Result traverseStartT(PKBField field1, PKBField field2);
    void traverseStartT(std::set<STMT_LO>* found, ParentNode* node, StatementType targetType);
    Result traverseEndT(PKBField field1, PKBField field2);
    Result traverseEndT(std::set<STMT_LO>* found, ParentNode* node, StatementType targetType);
    Result traverseAllT(StatementType field1, StatementType field2);
    Result traverseAll(StatementType type1, StatementType type2);
};
