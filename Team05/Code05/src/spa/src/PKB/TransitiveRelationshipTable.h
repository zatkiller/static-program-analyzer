#pragma once

#include <stdio.h>
#include <unordered_set>

#include "RelationshipRow.h"
#include "PKBRelationship.h"

using FieldRowResponse = std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>;

class TransitiveRelationshipTable {
public:
    explicit TransitiveRelationshipTable(PKBRelationship);

    /**
    * Checks whether the TransitiveRelationshipTable contains RELATIONSHIP(field1, field2).
    *
    * @param field1 the first program design entity in the relationship
    * @param field2 the second program design entity in the relationship
    *
    * @return whether the relationship is present in the TransitiveRelationshipTable
    */
    virtual bool contains(PKBField field1, PKBField field2) = 0;

    /**
    * Checks whether the TransitiveRelationshipTable contains RELATIONSHIP*(field1, field2),
    * where RELATIONSHIP = Follows || Parents
    *
    * @param field1 the first program design entity in the relationship
    * @param field2 the second program design entity in the relationship
    *
    * @return whether the relationship is present in the TransitiveRelationshipTable
    */
    virtual bool containsT(PKBField field1, PKBField field2) = 0;

    /**
    * Inserts a RELATIONSHIP(field1, field2) into the TransitiveRelationshipTable,
    * where RELATIONSHIP = Follows || Parents
    *
    * @param field1 the first program design entity in the relationship
    * @param field2 the second program design entity in the relationship
    */
    virtual void insert(PKBField field1, PKBField field2) = 0;

    /**
    * Gets all pairs of PKBFields that satisfy the provided RELATIONSHIP(field1, field2).
    * where RELATIONSHIP = Follows || Parents
    * 
    * @param field1 the first program design entity in the relationship
    * @param field2 the first program design entity in the relationship
    *
    * @return std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> all pairs of PKBFields
    * that satisfy RELATIONSHIP(field1, field2)
    * @see PKBField
    */
    virtual FieldRowResponse retrieve(PKBField field1, PKBField field2) = 0;
   
    /**
    * Gets all pairs of PKBFields that satisfy the provided RELATIONSHIP*(field1, field2).
    * where RELATIONSHIP = Follows || Parents
    *
    * @param field1 the first program design entity in the relationship
    * @param field2 the first program design entity in the relationship
    *
    * @return std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> all pairs of PKBFields
    * that satisfy RELATIONSHIP*(field1, field2)
    * @see PKBField
    */
    virtual FieldRowResponse retrieveT(PKBField field1, PKBField field2) = 0;

    /**
    * Checks whether the two PKBFields provided can exist in the TransitiveRelationshipTable.
    * 
    * @param field1 the first program design entity in the relationship
    * @param field2 the first program design entity in the relationship
    * @return bool
    */
    virtual bool isInsertOrContainsValid(PKBField field1, PKBField field2) = 0;

    /**
    * Checks whether the two PKBFields provided can be retrieved from the TransitiveRelationshipTable.
    *
    * @param field1 the first program design entity in the relationship
    * @param field2 the first program design entity in the relationship
    * @return bool
    */
    virtual bool isRetrieveValid(PKBField field1, PKBField field2) = 0;

    /**
    * Retrieves the type of relationships the TransitiveRelationshipTable stores.
    *
    * @return type of relationship
    */
    PKBRelationship getType();


protected:
    PKBRelationship type;
};
