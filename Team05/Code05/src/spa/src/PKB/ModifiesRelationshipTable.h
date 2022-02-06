#pragma once

#include <stdio.h>
#include <unordered_set>

#include "RelationshipTable.h"

class ModifiesRelationshipTable : public RelationshipTable {
public:
    ModifiesRelationshipTable();

    // count of an item in a set can only be 0 or 1
    bool contains(PKBField, PKBField);
    void insert(PKBField, PKBField);
    std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> retrieve(PKBField, PKBField);
};
