#include <stdio.h>
#include <unordered_set>

#include "ModifiesRelationshipTable.h"

ModifiesRelationshipTable::ModifiesRelationshipTable() : RelationshipTable{ PKBRelationship::MODIFIES } {};

// to ensure that both parameters are valid for ModifiesS or ModifiesP for retrieve or contain
bool ModifiesRelationshipTable::isRetrieveValid(PKBField field1, PKBField field2) {
    return (field1.entityType == PKBEntityType::PROCEDURE ||
        field1.entityType == PKBEntityType::STATEMENT) &&
        (field2.entityType == PKBEntityType::VARIABLE);
}

bool ModifiesRelationshipTable::isInsertOrContainsValid(PKBField field1, PKBField field2) {
    return (field1.isValidConcrete(PKBEntityType::STATEMENT) || field1.isValidConcrete(PKBEntityType::PROCEDURE))
        && field2.isValidConcrete(PKBEntityType::VARIABLE);
}

bool ModifiesRelationshipTable::contains(PKBField field1, PKBField field2) {
    if (!isInsertOrContainsValid(field1, field2)) return false;

    return rows.count(RelationshipRow(field1, field2)) == 1;
}

void ModifiesRelationshipTable::insert(PKBField field1, PKBField field2) {
    if (!isInsertOrContainsValid(field1, field2)) return;

    rows.insert(RelationshipRow(field1, field2));
}

std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>
ModifiesRelationshipTable::retrieve(PKBField field1, PKBField field2) {
    std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> res;
    if (!isRetrieveValid(field1, field2)) return res;

    if (!(field1.fieldType == PKBFieldType::CONCRETE) &&
        !(field2.fieldType == PKBFieldType::CONCRETE)) {
        for (auto row : rows) {
            RelationshipRow current = row;
            PKBField first = current.getFirst();
            PKBField second = current.getSecond();

            if (first.entityType == field1.entityType && second.entityType == field2.entityType) {
                res.insert(std::vector<PKBField>{first, second});
            }
        }
    } else if (field1.fieldType == PKBFieldType::CONCRETE && !(field2.fieldType == PKBFieldType::CONCRETE)) {
        for (auto row : rows) {
            RelationshipRow current = row;
            PKBField first = current.getFirst();
            PKBField second = current.getSecond();

            if (first == field1 && second.entityType == field2.entityType) {
                res.insert(std::vector<PKBField>{first, second});
            }
        }
    } else if (!(field1.fieldType == PKBFieldType::CONCRETE) && field2.fieldType == PKBFieldType::CONCRETE) {
        for (auto row : rows) {
            RelationshipRow current = row;
            PKBField first = current.getFirst();
            PKBField second = current.getSecond();

            if (first.entityType == field1.entityType && second == field2) {
                res.insert(std::vector<PKBField>{first, second});
            }
        }
    } else {
        for (auto row : rows) {
            RelationshipRow current = row;
            PKBField first = current.getFirst();
            PKBField second = current.getSecond();

            if (first == field1 && second == field2) {
                res.insert(std::vector<PKBField>{first, second});
            }
        }
    }

    return res;
}

