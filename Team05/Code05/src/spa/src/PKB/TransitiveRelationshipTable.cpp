#include <stdio.h>

#include "TransitiveRelationshipTable.h"

TransitiveRelationshipTable::TransitiveRelationshipTable(PKBRelationship rsType) : type(rsType) {}

PKBRelationship TransitiveRelationshipTable::getType() {
    return type;
}
