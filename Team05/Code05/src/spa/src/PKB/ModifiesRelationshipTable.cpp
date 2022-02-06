#include <stdio.h>
#include <unordered_set>

#include "ModifiesRelationshipTable.h"

ModifiesRelationshipTable::ModifiesRelationshipTable() : RelationshipTable{ PKBRelationship::MODIFIES } {};

// count of an item in a set can only be 0 or 1
bool ModifiesRelationshipTable::contains(PKBField entity1, PKBField entity2) {
    return rows.count(RelationshipRow(entity1, entity2)) == 1;
}

void ModifiesRelationshipTable::insert(PKBField entity1, PKBField entity2) {
    rows.insert(RelationshipRow(entity1, entity2));
}

std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> 
ModifiesRelationshipTable::retrieve(PKBField field1, PKBField field2) {
    std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> res;
    
    if (!field1.isConcrete && !field2.isConcrete) {
        for (auto iter = rows.begin(); iter != rows.end(); ++iter) {
            std::vector<PKBField> temp;

            RelationshipRow current = *iter;
            PKBField first = current.getFirst();
            PKBField second = current.getSecond();

            if (first.tag == field1.tag && second.tag == field2.tag) {
                temp.push_back(first);
                temp.push_back(second);
                res.insert(temp);
            }
        }
    } else if (field1.isConcrete && !field2.isConcrete) {
        for (auto iter = rows.begin(); iter != rows.end(); ++iter) {
            std::vector<PKBField> temp;

            RelationshipRow current = *iter;
            PKBField first = current.getFirst();
            PKBField second = current.getSecond();

            if (first == field1 && second.tag == field2.tag) {
                temp.push_back(first);
                temp.push_back(second);
                res.insert(temp);
            }
        }
    } else if (!field1.isConcrete && field2.isConcrete) {
        for (auto iter = rows.begin(); iter != rows.end(); ++iter) {
            std::vector<PKBField> temp;

            RelationshipRow current = *iter;
            PKBField first = current.getFirst();
            PKBField second = current.getSecond();

            if (first.tag == field1.tag && second == field2) {
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
