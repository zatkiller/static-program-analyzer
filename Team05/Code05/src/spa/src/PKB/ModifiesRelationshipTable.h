#pragma once

#include <stdio.h>
#include <unordered_set>

#include "RelationshipTable.h"

/**
* A data structure to store Modifies program design abstractions as RelationshipRows. Inherits RelationshipTable.
*/
class ModifiesRelationshipTable : public RelationshipTable {
public:
    ModifiesRelationshipTable();

    /**
    * Checks whether the ModifiesRelationshipTable contains a RelationshipRow representing 
    * Modifies(entity1, entity2).
    *
    * @param entity1 the first program design entity in the Modifies relationship
    * @param entity2 the second program design entity in the Modifies relationship
    *
    * @returns whether the relationship is present in the ModifiesRelationshipTable
    */
    bool contains(PKBField entity1, PKBField entity2);

    /**
    * Inserts a RelationshipRow representing Modifies(entity1, entity2) into the ModifiesRelationshipTable. 
    * If the first entity is not a Statement or a Procedure, or if the second one is not a statement,
    * no inserts will be done.
    *
    * @param entity1 the first program design entity in the Modifies relationship
    * @param entity2 the second program design entity in the Modifies relationship
    */
    void insert(PKBField entity1, PKBField entity2);

    bool isInsertValid(PKBField field1, PKBField field2);
    bool isContainsOrRetrieveValid(PKBField field1, PKBField field2);

    /**
    * Retrieves all RelationshipRows matching Modifies(field1, field2).
    *
    * @param field1 the first program design entity in the Modifies relationship
    * @param field2 the second program design entity in the Modifies relationship
    *
    * @return a set of vectors of PKBFields, where each vector represents the two program design entities
    * in a Modifies relationship. Modifies(field1, field2) -> [field1, field2]
    */
    std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> retrieve(PKBField field1, PKBField field2);
};
