#pragma once

#include <unordered_set>

#include "ParentGraph.h"
#include "TransitiveRelationshipTable.h"

using FieldRowResponse = std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>;

//class ParentRelationshipTable : public TransitiveRelationshipTable {
//// class ParentRelationshipTable : public TransitiveRelationshipTable {
//public:
//    ParentRelationshipTable();
//
//    bool contains(PKBField entity1, PKBField entity2);
//    void insert(PKBField entity1, PKBField entity2);
//    FieldRowResponse retrieve(PKBField entity1, PKBField entity2);
//    bool containsT(PKBField entity1, PKBField entity2);
//    bool isInsertOrContainsValid(PKBField field1, PKBField field2);
//    bool isRetrieveValid(PKBField field1, PKBField field2);
//
//    // TODO(Patrick): implement retrieveT with graphs
//    // FieldRowResponse retrieveT(PKBField entity1, PKBField entity2);
//// private:
////    std::unique_ptr<ParentGraph> parentGraph;
//};
