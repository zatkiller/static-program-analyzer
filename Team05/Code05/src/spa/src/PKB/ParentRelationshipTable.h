#pragma once

#include <unordered_set>
#include <memory>

#include "logging.h"
#include "ParentGraph.h"
#include "TransitiveRelationshipTable.h"

using FieldRowResponse = std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>;

class ParentRelationshipTable : public TransitiveRelationshipTable {
public:
    ParentRelationshipTable();

    bool contains(PKBField field1, PKBField field2);
    void insert(PKBField field1, PKBField field2);
    FieldRowResponse retrieve(PKBField field1, PKBField field2);
    bool containsT(PKBField field1, PKBField field2);
    FieldRowResponse retrieveT(PKBField field1, PKBField field2);

 private:
    std::unique_ptr<ParentGraph> parentGraph;
    bool isInsertOrContainsValid(PKBField, PKBField);
    bool isRetrieveValid(PKBField field1, PKBField field2);
};
