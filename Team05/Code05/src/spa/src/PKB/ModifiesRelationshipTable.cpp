#pragma once

#include <stdio.h>
#include <set>

#include "ModifiesRelationshipTable.h"

ModifiesRelationshipTable::ModifiesRelationshipTable() : RelationshipTable{ PKBRelationship::MODIFIES } {};

// count of an item in a set can only be 0 or 1
bool ModifiesRelationshipTable::contains(PKBField entity1, PKBField entity2) {
	return rows.count(RelationshipRow(entity1, entity2)) == 1;
}

void ModifiesRelationshipTable::insert(PKBField entity1, PKBField entity2) {
	rows.insert(RelationshipRow(entity1, entity2));
}
