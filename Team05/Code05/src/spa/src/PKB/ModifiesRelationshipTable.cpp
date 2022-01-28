#pragma once

#include <stdio.h>
#include <set>

#include "ModifiesRelationshipTable.h"


ModifiesRelationshipTable::ModifiesRelationshipTable(int t) : RelationshipTable{ t } {};

// TODO: replace <int> with <PKBField>
// count of an item in a set can only be 0 or 1
bool ModifiesRelationshipTable::contains(int entity1, int entity2) {
	return rows.count(RelationshipRow(entity1, entity2)) == 1;
}

void ModifiesRelationshipTable::insert(int entity1, int entity2) {
	rows.insert(RelationshipRow(entity1, entity2));
}