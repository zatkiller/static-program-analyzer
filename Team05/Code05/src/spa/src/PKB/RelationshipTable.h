#pragma once

#include <stdio.h>
#include <unordered_set>

#include "logging.h"
#include "RelationshipRow.h"
#include "PKBRelationship.h"

using FieldRowResponse = std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>;

/**
* A data structure to store program design abstractions as RelationshipRows. Base class of *RelationshipTables.
*/
class RelationshipTable {
public:
    explicit RelationshipTable(PKBRelationship);

    /**
    * Checks whether the RelationshipTable contains a RelationshipRow representing
    * Relationship(entity1, entity2).
    *
    * @param entity1 the first program design entity in the relationship
    * @param entity2 the second program design entity in the relationship
    *
    * @return whether the relationship is present in the RelationshipTable
    */
    bool contains(PKBField entity1, PKBField entity2);

    /**
    * Inserts a RelationshipRow representing Relationship(entity1, entity2) into the RelationshipTable.
    *
    * @param entity1 the first program design entity in the relationship
    * @param entity2 the second program design entity in the relationship
    */
    void insert(PKBField entity1, PKBField entity2);

    FieldRowResponse retrieve(PKBField entity1, PKBField entity2);

    bool isInsertOrContainsValid(PKBField field1, PKBField field2);
    bool isRetrieveValid(PKBField field1, PKBField field2);

    /**
    * Retrieves the type of relationships the RelationshipTable stores.
    *
    * @return type of relationship
    */
    PKBRelationship getType();

    int getSize();

protected:
    std::unordered_set<RelationshipRow, RelationshipRowHash> rows;
    PKBRelationship type;
};
