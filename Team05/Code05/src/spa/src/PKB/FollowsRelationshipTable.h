#pragma once

#include <unordered_set>

#include "RelationshipTable.h"

using FieldRowResponse = std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>;

class FollowsRelationshipTable : public RelationshipTable {
public:
    FollowsRelationshipTable();

    bool contains(PKBField entity1, PKBField entity2);
    void insert(PKBField entity1, PKBField entity2);
    FieldRowResponse retrieve(PKBField entity1, PKBField entity2);
};
