#pragma once

#include <stdio.h>
#include <unordered_set>

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
    * @returns whether the relationship is present in the RelationshipTable
    */
    virtual bool contains(PKBField entity1, PKBField entity2) = 0;

    /**
    * Inserts a RelationshipRow representing Relationship(entity1, entity2) into the RelationshipTable.
    *
    * @param entity1 the first program design entity in the relationship
    * @param entity2 the second program design entity in the relationship
    */
    virtual void insert(PKBField entity1, PKBField entity2) = 0;

    virtual FieldRowResponse retrieve(PKBField entity1, PKBField entity2) = 0;

    /** 
    * Retrieves the type of relationships the RelationshipTable stores.
    * 
    * @returns type of relationship
    */
    PKBRelationship getType();
    int getSize();

protected:
    std::unordered_set<RelationshipRow, RelationshipRowHash> rows;
    PKBRelationship type; 
};

class TransitiveRelationshipTable : public RelationshipTable {
public:
    using RelationshipTable::RelationshipTable;

    virtual FieldRowResponse retrieveT(PKBField entity1, PKBField entity2) = 0;
};
