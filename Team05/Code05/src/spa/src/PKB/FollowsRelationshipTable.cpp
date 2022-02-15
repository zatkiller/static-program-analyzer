#include <unordered_set>

#include "FollowsRelationshipTable.h"

FollowsRelationshipTable::FollowsRelationshipTable() : TransitiveRelationshipTable{ PKBRelationship::FOLLOWS } {
    followsGraph = std::make_unique<FollowsGraph>();
};

bool FollowsRelationshipTable::isRetrieveValid(PKBField entity1, PKBField entity2) {
    return entity1.entityType == PKBEntityType::STATEMENT &&
        entity2.entityType == PKBEntityType::STATEMENT;
}

bool FollowsRelationshipTable::contains(PKBField entity1, PKBField entity2) {
    if (!isInsertOrContainsValid(entity1, entity2)) {
        throw "Only concrete statements are allowed!";
    }

    return followsGraph->getContains(entity1, entity2);
}

void FollowsRelationshipTable::insert(PKBField entity1, PKBField entity2) {
    if (!isInsertOrContainsValid(entity1, entity2)) {
        throw "Only concrete statements can be inserted into the Follows table!";
    }

    followsGraph->addEdge(*entity1.getContent<STMT_LO>(), *entity2.getContent<STMT_LO>());
}

FieldRowResponse FollowsRelationshipTable::retrieve(PKBField entity1, PKBField entity2) {
    // Both fields have to be a statement type
    if (!isRetrieveValid(entity1, entity2)) {
        throw "Follows relationship is only defined for statements!";
    }

    return followsGraph->getFollows(entity1, entity2);
}

bool FollowsRelationshipTable::containsT(PKBField entity1, PKBField entity2) {
    if (entity1.entityType != PKBEntityType::STATEMENT || entity2.entityType != PKBEntityType::STATEMENT) {
        throw "Input fields must be statements!";
    }
    if (entity1.fieldType != PKBFieldType::CONCRETE || entity2.fieldType != PKBFieldType::CONCRETE) {
        throw "Input fields must be concrete!";
    }

    return followsGraph->getContainsT(entity1, entity2);
}

FieldRowResponse FollowsRelationshipTable::retrieveT(PKBField entity1, PKBField entity2) {
    // for any fields that are wildcards, convert them into declarations of all types
    if (entity1.fieldType == PKBFieldType::WILDCARD) {
        entity1.fieldType = PKBFieldType::DECLARATION;
        entity1.statementType = StatementType::All;
    }
    
    if (entity2.fieldType == PKBFieldType::WILDCARD) {
        entity2.fieldType = PKBFieldType::DECLARATION;
        entity2.statementType = StatementType::All;
    }

    if (entity1.fieldType == PKBFieldType::CONCRETE && 
        entity2.fieldType == PKBFieldType::CONCRETE) {
        return this->containsT(entity1, entity2)
            ? FieldRowResponse{ {{entity1, entity2}} }
            : FieldRowResponse{};
    }

    return followsGraph->getFollowsT(entity1, entity2);
}

bool FollowsRelationshipTable::isInsertOrContainsValid(PKBField field1, PKBField field2) {
    return field1.isValidConcrete(PKBEntityType::STATEMENT) && field2.isValidConcrete(PKBEntityType::STATEMENT);
}
