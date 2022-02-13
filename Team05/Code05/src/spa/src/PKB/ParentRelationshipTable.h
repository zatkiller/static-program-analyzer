#pragma once

#include <unordered_set>

#include "RelationshipTable.h"

using FieldRowResponse = std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>;

class ParentRelationshipTable : public RelationshipTable {
public:
    ParentRelationshipTable();

    bool contains(PKBField entity1, PKBField entity2);
    void insert(PKBField entity1, PKBField entity2);
    FieldRowResponse retrieve(PKBField entity1, PKBField entity2);
    bool containsT(PKBField entity1, PKBField entity2);

    // TODO(Patrick): implement retrieveT with graphs
    // FieldRowResponse retrieveT(PKBField entity1, PKBField entity2);
};