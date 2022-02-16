#include "UsesRelationshipTable.h"

UsesRelationshipTable::UsesRelationshipTable() : RelationshipTable{ PKBRelationship::USES } {};

bool UsesRelationshipTable::isRetrieveValid(PKBField entity1, PKBField entity2) {
    return (entity1.entityType == PKBEntityType::PROCEDURE ||
        entity1.entityType == PKBEntityType::STATEMENT) &&
        (entity2.entityType == PKBEntityType::VARIABLE);
}

bool UsesRelationshipTable::contains(PKBField entity1, PKBField entity2) {
    if (!isInsertOrContainsValid(entity1, entity2)) {
        throw "Both fields must be concrete and be of the correct types!";
    }

    return rows.count(RelationshipRow(entity1, entity2));
}

void UsesRelationshipTable::insert(PKBField entity1, PKBField entity2) {
    if (!isInsertOrContainsValid(entity1, entity2)) {
        throw "Only concrete fields of the correct types can be inserted into the Uses table!";
    }

    rows.insert(RelationshipRow(entity1, entity2));
}

FieldRowResponse UsesRelationshipTable::retrieve(PKBField entity1, PKBField entity2) {
    if (!isRetrieveValid(entity1, entity2)) {
        throw "First field in Uses must either be a statement or a procedure, and the second must be a variable!";
    }

    PKBFieldType fieldType1 = entity1.fieldType;
    PKBFieldType fieldType2 = entity2.fieldType;

    FieldRowResponse res;

    if (fieldType1 == PKBFieldType::CONCRETE && fieldType2 == PKBFieldType::CONCRETE) {
        if (this->contains(entity1, entity2)) res.insert({ entity1, entity2 });

    } else if (fieldType1 == PKBFieldType::CONCRETE && fieldType2 != PKBFieldType::CONCRETE) {
        for (auto row : rows) {
            RelationshipRow curr = row;
            PKBField first = curr.getFirst();
            PKBField second = curr.getSecond();

            if (first == entity1) {
                res.insert({ first,second });
            }
        }
        // either statement declaration or wildcard, or procedure wildcard/declaration
    } else if (fieldType1 != PKBFieldType::CONCRETE && fieldType2 == PKBFieldType::CONCRETE) {
        // field1 cannot be both statement and wildcard, invalid syntax
        for (auto row : rows) {
            RelationshipRow curr = row;
            PKBField first = curr.getFirst();
            PKBField second = curr.getSecond();

            // If first declaration looking for Procedures
            if (entity1.entityType == PKBEntityType::PROCEDURE && first.entityType == PKBEntityType::PROCEDURE) {
                if (second == entity2) {
                    res.insert({ first,second });
                }
            }

            if (entity1.entityType == PKBEntityType::STATEMENT && first.entityType == PKBEntityType::STATEMENT) {
                if (second == entity2) {
                    if (entity1.statementType.value() == StatementType::All || entity1.statementType == first.getContent<STMT_LO>()->type.value()) {
                        res.insert({ first,second });
                    }
                }
            }
        }
    } else { // both declaration/wild
        for (auto row : rows) {
            RelationshipRow curr = row;
            PKBField first = curr.getFirst();
            PKBField second = curr.getSecond();

            // If first declaration looking for Procedures
            if (entity1.entityType == PKBEntityType::PROCEDURE && first.entityType == PKBEntityType::PROCEDURE) {
                res.insert({ first,second });
            }

            if (entity1.entityType == PKBEntityType::STATEMENT && first.entityType == PKBEntityType::STATEMENT) {
                if (entity1.statementType.value() == StatementType::All || entity1.statementType == first.getContent<STMT_LO>()->type.value()) {
                    res.insert({ first,second });
                }
            }
        }
    }

    return res;
}

bool UsesRelationshipTable::isInsertOrContainsValid(PKBField field1, PKBField field2) {
    return (field1.isValidConcrete(PKBEntityType::STATEMENT) || field1.isValidConcrete(PKBEntityType::PROCEDURE))
        && field2.isValidConcrete(PKBEntityType::VARIABLE);
}
