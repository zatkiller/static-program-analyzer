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
    * Relationship(field1, field2). If either fields are not concrete or if a statement field 
    * has no corresponding row in statement table, return false.
    *
    * @param field1 the first program design entity in the relationship
    * @param field2 the second program design entity in the relationship
    *
    * @return whether the relationship is present in the RelationshipTable
    */
    bool contains(PKBField field1, PKBField field2);

    /**
    * Inserts a RelationshipRow representing Relationship(field1, field2) into the RelationshipTable.
    *
    * @param field1 the first program design entity in the relationship
    * @param field2 the second program design entity in the relationship
    */
    void insert(PKBField field1, PKBField field2);

    /**
    * Retrieves all pairs of PKBFields in table that satisfies the parameters. If a statement field 
    * has no corresponding row in statement table, or if the first parameter is a wildcard, return false.
    * 
    * @param field1 the first program design entity in the query
    * @param field2 the second program design entity in the query
    * @return FieldRowResponse
    * 
    * @see PKBField
    */
    FieldRowResponse retrieve(PKBField field1, PKBField field2);

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
