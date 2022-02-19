#pragma once

#include <unordered_set>
#include <memory>

#include "logging.h"
#include "ParentGraph.h"
#include "TransitiveRelationshipTable.h"

using FieldRowResponse = std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>;

/**
* A data structure to store Parent and Parent* program design abstractions as ParentNodes in a ParentGraph.
* Inherits from TransitiveRelationshipTable
* 
* @see ParentNode, ParentGraph, TransitiveRelationshipTable
*/
class ParentRelationshipTable : public TransitiveRelationshipTable {
public:
    ParentRelationshipTable();

    /**
    * Checks whether the FollowsRelationshipTable contains Parent(field1, field2).
    *
    * @param field1 the first STMT_LO in a Parent(u,v) query wrapped in a PKBField
    * @param field2 the second STMT_LO in a Parent(u,v) query wrapped in a PKBField
    *
    * @return bool whether Parent(field1, field2) is in the graph
    * 
    * @see PKBField
    */
    bool contains(PKBField field1, PKBField field2);

    /**
    * Inserts into ParentGraph an edge representing Parent(field1, field2).
    * If the two provided entities are not valid statements or are not concrete, no insert will be done
    * and an error will be thrown.
    *
    * @param field1 the first STMT_LO in a Parent(u,v) relationship wrapped in a PKBField
    * @param field2 the second STMT_LO in a Parent(u,v) relationship wrapped in a PKBField
    * 
    * @see PKBField
    */
    void insert(PKBField field1, PKBField field2);

    /**
    * Retrieves all pairs of statements that satisfies Parent(field1, field2).
    *
    * @param field1 the first STMT_LO in a Parent(u,v) query wrapped in a PKBField
    * @param field2 the second STMT_LO in a Parent(u,v) query wrapped in a PKBField
    *
    * @return std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> an unordered set of vectors of PKBFields,
    *   where each vector represents the two program design entities in a Parent relationship,
    *   i.e. Parent(field1, field2) -> [field1, field2].
    * 
    * @see PKBField
    */
    FieldRowResponse retrieve(PKBField field1, PKBField field2);

    /**
    * Checks whether the ParentRelationshipTable contains Parent*(field1, field2).
    *
    * @param field1 the first STMT_LO in a Parent*(u,v) query wrapped in a PKBField
    * @param field2 the second STMT_LO in a Parent*(u,v) query wrapped in a PKBField
    *
    * @return bool whether Parent*(field1, field2) is in the graph
    * 
    * @see PKBField
    */
    bool containsT(PKBField field1, PKBField field2);

    /**
    * Retrieves all pairs of statements that satisfies Parent*(field1, field2).
    *
    * @param field1 the first STMT_LO in a Parent*(u,v) query wrapped in a PKBField
    * @param field2 the second STMT_LO in a Parent*(u,v) query wrapped in a PKBField
    *
    * @return std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> an unordered set of vectors of PKBFields,
    *   where each vector represents the two program design entities in a Parent* relationship,
    *   i.e. Parent*(field1, field2) -> [field1, field2].
    * 
    * @see PKBField
    */
    FieldRowResponse retrieveT(PKBField field1, PKBField field2);

 private:
    std::unique_ptr<ParentGraph> parentGraph;

    /**
    * Checks if the two PKBFields provided can be inserted into the table (whether it can exist in the table).
    * Checks that the fields are concrete and have valid STMT_LOs.
    *
    * @param field1 the first PKBField
    * @param field2 the second PKBField
    *
    * @return bool
    * 
    * @see PKBField
    */
    bool isInsertOrContainsValid(PKBField, PKBField);

    /**
    * Checks if the two PKBFields provided can be retrieved from the table.
    *
    * @param field1 the first PKBField
    * @param field2 the second PKBField
    *
    * @return bool
    * 
    * @see PKBField
    */
    bool isRetrieveValid(PKBField field1, PKBField field2);
};
