#pragma once

#include <unordered_set>

#include "ParentGraph.h"
#include "TransitiveRelationshipTable.h"

using FieldRowResponse = std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>;

// TODO(patrick) remove rows and implement all functions with graph
class ParentRelationshipTable : public TransitiveRelationshipTable {
// class ParentRelationshipTable : public TransitiveRelationshipTable {
public:
    ParentRelationshipTable();

    bool contains(PKBField field1, PKBField field2);
    void insert(PKBField field1, PKBField field2);
    FieldRowResponse retrieve(PKBField field1, PKBField field2);
    bool containsT(PKBField field1, PKBField field2);
    bool isInsertOrContainsValid(PKBField field1, PKBField field2);
    bool isRetrieveValid(PKBField field1, PKBField field2);

    // TODO(Patrick): implement retrieveT with graphs
    FieldRowResponse retrieveT(PKBField field1, PKBField field2);
// private:
//    std::unique_ptr<ParentGraph> parentGraph;
    std::unordered_set<RelationshipRow, RelationshipRowHash> rows;

};
