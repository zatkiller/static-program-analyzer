#include <stdio.h>

#include "RelationshipTable.h"

RelationshipTable::RelationshipTable(PKBRelationship rsType) : type(rsType) {}

bool RelationshipTable::isRetrieveValid(PKBField field1, PKBField field2) {
    // both Modifies and Uses cannot accept a wildcard as its first parameter
    return field1.fieldType != PKBFieldType::WILDCARD &&
        (field1.entityType == PKBEntityType::PROCEDURE ||
            field1.entityType == PKBEntityType::STATEMENT) &&
        (field2.entityType == PKBEntityType::VARIABLE);
}

bool RelationshipTable::isInsertOrContainsValid(PKBField field1, PKBField field2) {
    return (field1.isValidConcrete(PKBEntityType::STATEMENT) || field1.isValidConcrete(PKBEntityType::PROCEDURE))
        && field2.isValidConcrete(PKBEntityType::VARIABLE);
}

bool RelationshipTable::contains(PKBField field1, PKBField field2) {
    if (!isInsertOrContainsValid(field1, field2)) {
        Logger(Level::ERROR) <<
            "RelationshipTable can only contain concrete fields and STATEMENT or PROCEDURE entity types.";
        return false;
    };

    return rows.count(RelationshipRow(field1, field2)) == 1;
}

void RelationshipTable::insert(PKBField field1, PKBField field2) {
    if (!isInsertOrContainsValid(field1, field2)) {
        Logger(Level::ERROR) <<
            "RelationshipTable only allow inserts of concrete fields and STATEMENT or PROCEDURE entity types.";
        return;
    }

    rows.insert(RelationshipRow(field1, field2));
}

FieldRowResponse RelationshipTable::retrieve(PKBField field1, PKBField field2) {
    PKBFieldType fieldType1 = field1.fieldType;
    PKBFieldType fieldType2 = field2.fieldType;

    FieldRowResponse res;

    if (!isRetrieveValid(field1, field2)) {
        Logger(Level::ERROR) <<
            "Only fields of STATEMENT or PROCEDURE entity types can be retrieved from RelationshipTable.";
        return res;
    }

    if (fieldType1 == PKBFieldType::CONCRETE && fieldType2 == PKBFieldType::CONCRETE) {
        if (this->contains(field1, field2)) res.insert({ field1, field2 });

    } else if (fieldType1 == PKBFieldType::CONCRETE && fieldType2 != PKBFieldType::CONCRETE) {
        for (auto row : rows) {
            RelationshipRow curr = row;
            PKBField first = curr.getFirst();
            PKBField second = curr.getSecond();

            if (first == field1) {
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
            if (field1.entityType == PKBEntityType::PROCEDURE && first.entityType == PKBEntityType::PROCEDURE) {
                if (second == field2) {
                    res.insert({ first,second });
                }
            }

            if (field1.entityType == PKBEntityType::STATEMENT && first.entityType == PKBEntityType::STATEMENT) {
                if (second == field2) {
                    if (field1.statementType.value() == StatementType::All || field1.statementType == first.getContent<STMT_LO>()->type.value()) {
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
            if (field1.entityType == PKBEntityType::PROCEDURE && first.entityType == PKBEntityType::PROCEDURE) {
                res.insert({ first,second });
            }

            if (field1.entityType == PKBEntityType::STATEMENT && first.entityType == PKBEntityType::STATEMENT) {
                if (field1.statementType.value() == StatementType::All || field1.statementType == first.getContent<STMT_LO>()->type.value()) {
                    res.insert({ first,second });
                }
            }
        }
    }

    return res;
}

PKBRelationship RelationshipTable::getType() {
    return type;
}

int RelationshipTable::getSize() {
    return rows.size();
}
