#include <unordered_set>

#include "FollowsRelationshipTable.h"

FollowsRelationshipTable::FollowsRelationshipTable() : TransitiveRelationshipTable{ PKBRelationship::FOLLOWS } {
    followsGraph = std::make_unique<FollowsGraph>();
};

void FollowsRelationshipTable::insert(PKBField field1, PKBField field2) {
    if (!isInsertOrContainsValid(field1, field2)) {
        Logger(Level::ERROR) << "Only concrete statements can be inserted into the Follows table!";
        return;
    }

    followsGraph->addEdge(*field1.getContent<STMT_LO>(), *field2.getContent<STMT_LO>());
}

bool FollowsRelationshipTable::contains(PKBField field1, PKBField field2) {
    if (!isInsertOrContainsValid(field1, field2)) {
        Logger(Level::ERROR) << "Only concrete statements can be inserted into the Follows table!";
        return false;
    }

    return followsGraph->getContains(field1, field2);
}

bool FollowsRelationshipTable::containsT(PKBField field1, PKBField field2) {
    if (!isInsertOrContainsValid(field1, field2)) {
        Logger(Level::ERROR) <<
            "FollowsRelationshipTable can only contain concrete fields and STATEMENT entity types.";
        return false;
    }

    return followsGraph->getContainsT(field1, field2);
}

FieldRowResponse FollowsRelationshipTable::retrieve(PKBField field1, PKBField field2) {
    // Both fields have to be a statement type
    if (!isRetrieveValid(field1, field2)) {
        Logger(Level::ERROR) <<
            "Only STATEMENT entity types can be retrieved from FollowsRelationshipTable.";
        return FieldRowResponse{ };
    }

    // for any fields that are wildcards, convert them into declarations of all types
    if (field1.fieldType == PKBFieldType::WILDCARD) {
        field1.fieldType = PKBFieldType::DECLARATION;
        field1.statementType = StatementType::All;
    }

    if (field2.fieldType == PKBFieldType::WILDCARD) {
        field2.fieldType = PKBFieldType::DECLARATION;
        field2.statementType = StatementType::All;
    }

    if (field1.fieldType == PKBFieldType::CONCRETE &&
        field2.fieldType == PKBFieldType::CONCRETE) {
        return this->contains(field1, field2)
            ? FieldRowResponse{ {{field1, field2}} }
        : FieldRowResponse{};
    }

    return followsGraph->getFollows(field1, field2);
}

FieldRowResponse FollowsRelationshipTable::retrieveT(PKBField field1, PKBField field2) {
    // Both fields have to be a statement type
    if (!isRetrieveValid(field1, field2)) {
        Logger(Level::ERROR) <<
            "Only STATEMENT entity types can be retrieved from FollowsRelationshipTable.";
        return FieldRowResponse{ };
    }
    
    // for any fields that are wildcards, convert them into declarations of all types
    if (field1.fieldType == PKBFieldType::WILDCARD) {
        field1.fieldType = PKBFieldType::DECLARATION;
        field1.statementType = StatementType::All;
    }

    if (field2.fieldType == PKBFieldType::WILDCARD) {
        field2.fieldType = PKBFieldType::DECLARATION;
        field2.statementType = StatementType::All;
    }

    if (field1.fieldType == PKBFieldType::CONCRETE &&
        field2.fieldType == PKBFieldType::CONCRETE) {
        return this->containsT(field1, field2)
            ? FieldRowResponse{ {{field1, field2}} }
        : FieldRowResponse{};
    }

    return followsGraph->getFollowsT(field1, field2);
}

bool FollowsRelationshipTable::isInsertOrContainsValid(PKBField field1, PKBField field2) {
    return field1.isValidConcrete(PKBEntityType::STATEMENT) && field2.isValidConcrete(PKBEntityType::STATEMENT);
}

bool FollowsRelationshipTable::isRetrieveValid(PKBField field1, PKBField field2) {
    return field1.entityType == PKBEntityType::STATEMENT &&
        field2.entityType == PKBEntityType::STATEMENT;
}
