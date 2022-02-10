#include <stdio.h>
#include <tuple>

#include "PKBField.h"
#include "RelationshipRow.h"

RelationshipRow::RelationshipRow(PKBField e1, PKBField e2): entity1(e1), entity2(e2) {}

bool RelationshipRow::operator == (const RelationshipRow& row) const {
    return entity1 == row.entity1 && entity2 == row.entity2;
}

PKBField RelationshipRow::getFirst() const {
    return entity1;
}

PKBField RelationshipRow::getSecond() const {
    return entity2;
}

size_t RelationshipRowHash::operator() (const RelationshipRow& other) const {
    PKBField ent1 = other.getFirst();
    PKBField ent2 = other.getSecond();

    PKBEntityType entityType1 = ent1.entityType;
    PKBEntityType entityType2 = ent2.entityType;

    return (std::hash<PKBEntityType>()(entityType1) ^ std::hash<PKBEntityType>()(entityType2));
}
