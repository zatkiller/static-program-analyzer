#include "ParentRelationshipTable.h"

 ParentRelationshipTable::ParentRelationshipTable() : TransitiveRelationshipTable{ PKBRelationship::PARENT } {
     parentGraph = std::make_unique<ParentGraph>();
 };

bool ParentRelationshipTable::isRetrieveValid(PKBField field1, PKBField entity2) {
    return field1.entityType == PKBEntityType::STATEMENT &&
        entity2.entityType == PKBEntityType::STATEMENT;
}

bool ParentRelationshipTable::isInsertOrContainsValid(PKBField field1, PKBField field2) {
    return field1.isValidConcrete(PKBEntityType::STATEMENT) && 
        field2.isValidConcrete(PKBEntityType::STATEMENT);
}

bool ParentRelationshipTable::contains(PKBField field1, PKBField field2) {
    if (!isInsertOrContainsValid(field1, field2)) {
        Logger(Level::ERROR) << "Only concrete statements can be inserted into the Parent table!";
        return false;
    } 
    
    return this->parentGraph->getContains(field1, field2);
}

void ParentRelationshipTable::insert(PKBField field1, PKBField field2) {
    if (!isInsertOrContainsValid(field1, field2)) {
        Logger(Level::ERROR) << "Only concrete statements can be inserted into the Parent table!";
        return;
    }

    STMT_LO first = *(field1.getContent<STMT_LO>());
    STMT_LO second = *(field2.getContent<STMT_LO>());

    this->parentGraph->addEdge(first, second);
}

FieldRowResponse ParentRelationshipTable::retrieve(PKBField field1, PKBField field2) {
    // Check both fields are statement types
    if (!isRetrieveValid(field1, field2)) {
        Logger(Level::ERROR) <<
            "Only concrete fields and STATEMENT entity types can be retrieved from ParentRelationshipTable";
        
        return FieldRowResponse{};
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

    return this->parentGraph->getParent(field1, field2);
}

bool ParentRelationshipTable::containsT(PKBField field1, PKBField field2) {
    if (!isInsertOrContainsValid(field1, field2)) {
        Logger(Level::ERROR) <<
            "ParentRelationshipTable can only contain concrete fields and STATEMENT entity types.";
        return false;
    }
    return this->parentGraph->getContainsT(field1, field2);
}

 FieldRowResponse ParentRelationshipTable::retrieveT(PKBField field1, PKBField field2) {
     // Treat all wildcards as declarations
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

     return this->parentGraph->getParentT(field1, field2);
 }
