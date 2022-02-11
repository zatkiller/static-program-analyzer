#include <stdio.h>
#include <unordered_set>

#include "ModifiesRelationshipTable.h"

ModifiesRelationshipTable::ModifiesRelationshipTable() : RelationshipTable{ PKBRelationship::MODIFIES } {};

// to ensure that both parameters are valid for ModifiesS or ModifiesP
bool arePKBFieldsValid(PKBField entity1, PKBField entity2) {
    return (entity1.entityType == PKBEntityType::PROCEDURE ||
        entity1.entityType == PKBEntityType::STATEMENT) &&
        (entity2.entityType == PKBEntityType::VARIABLE);
}

bool ModifiesRelationshipTable::contains(PKBField entity1, PKBField entity2) {
    if (!arePKBFieldsValid(entity1, entity2)) return false;

    return rows.count(RelationshipRow(entity1, entity2)) == 1;
}

void ModifiesRelationshipTable::insert(PKBField entity1, PKBField entity2) {
    if (!arePKBFieldsValid(entity1, entity2)) return;

    rows.insert(RelationshipRow(entity1, entity2));
}

std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> 
ModifiesRelationshipTable::retrieve(PKBField field1, PKBField field2) {
    std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> res;
    
    if (!(field1.fieldType == PKBFieldType::CONCRETE) && 
        !(field2.fieldType == PKBFieldType::CONCRETE)) {
        for (auto iter = rows.begin(); iter != rows.end(); ++iter) {
            std::vector<PKBField> temp;

            RelationshipRow current = *iter;
            PKBField first = current.getFirst();
            PKBField second = current.getSecond();

            if (first.entityType == field1.entityType && second.entityType == field2.entityType) {
                temp.push_back(first);
                temp.push_back(second);
                res.insert(temp);
            }
        }
    } else if (field1.fieldType == PKBFieldType::CONCRETE && !(field2.fieldType == PKBFieldType::CONCRETE)) {
        for (auto iter = rows.begin(); iter != rows.end(); ++iter) {
            std::vector<PKBField> temp;

            RelationshipRow current = *iter;
            PKBField first = current.getFirst();
            PKBField second = current.getSecond();

            if (first == field1 && second.entityType == field2.entityType) {
                temp.push_back(first);
                temp.push_back(second);
                res.insert(temp);
            }
        }
    } else if (!(field1.fieldType == PKBFieldType::CONCRETE) && field2.fieldType == PKBFieldType::CONCRETE) {
        for (auto iter = rows.begin(); iter != rows.end(); ++iter) {
            std::vector<PKBField> temp;

            RelationshipRow current = *iter;
            PKBField first = current.getFirst();
            PKBField second = current.getSecond();

            if (first.entityType == field1.entityType && second == field2) {
                temp.push_back(first);
                temp.push_back(second);
                res.insert(temp);
            }
        }
    } else {
        for (auto iter = rows.begin(); iter != rows.end(); ++iter) {
            std::vector<PKBField> temp;

            RelationshipRow current = *iter;
            PKBField first = current.getFirst();
            PKBField second = current.getSecond();

            if (first == field1 && second == field2) {
                temp.push_back(first);
                temp.push_back(second);
                res.insert(temp);
            }
        }
    }

    return res;
}
