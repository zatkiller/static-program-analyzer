#pragma once

#include <unordered_set>
#include <memory>
#include "RelationshipTable.h"
#include "FollowsGraph.h"

using FieldRowResponse = std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>;

/**
 * A data structure to store Follows and FollowsT program design abstractions as RelationshipRows. Inherits from RelationshipTable.
 */
class FollowsRelationshipTable : public TransitiveRelationshipTable {
public:
    FollowsRelationshipTable();

    /**
    * Checks whether the FollowsRelationshipTable contains a RelationshipRow representing
    * Follows(entity1, entity2).
    *
    * @param entity1 the first program design entity in the Follows relationship
    * @param entity2 the second program design entity in the Follows relationship
    *
    * @returns whether the relationship is present in the FollowsRelationshipTable
    */
    bool contains(PKBField entity1, PKBField entity2);

    /**
    * Inserts a RelationshipRow representing Follows(entity1, entity2) into the FollowsRelationshipTable.
    * If the two provided entities are not Statements or are not concrete in nature, no inserts will be done
    * and an error will be thrown.
    *
    * @param entity1 the first program design entity in the Follows relationship
    * @param entity2 the second program design entity in the Follows relationship
    */
    void insert(PKBField entity1, PKBField entity2);

    /**
    * Retrieves all RelationshipRows that match Follows(entity1, entity2).
    *
    * @param entity1 the first program design entity in the Follows relationship
    * @param entity2 the second program design entity in the Follows relationship
    *
    * @return an unordered set of vectors of PKBFields, where each vector represents the two program design
    * entities in a Follows relationship i.e. Follows(field1, field2) -> [field1, field2]
    */
    FieldRowResponse retrieve(PKBField entity1, PKBField entity2);
    bool containsT(PKBField entity1, PKBField entity2);

    /**
    * Retrieves all RelationshipRows that match Follows*(entity1, entity2).
    * 
    * @param entity1 the first program design entity in the Follows* relationship
    * @param entity2 the second program design entity in the Follows* relationship
    * 
    * @return an unordered set of vectors of PKBfields, where each vector represents the two program design 
    * entities in a Follows* relationship i.e. Follows*(field1, field2) -> [field1, field2].
    */
    FieldRowResponse retrieveT(PKBField entity1, PKBField entity2);

private:
    std::unique_ptr<FollowsGraph> followsGraph;
};

