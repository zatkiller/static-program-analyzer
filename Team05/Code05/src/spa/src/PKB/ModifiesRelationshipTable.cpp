#pragma once

#include <stdio.h>
#include <set>
#include <iostream>
#include "ModifiesRelationshipTable.h"

// TODO: Replace 1 with PKBRelationship enum
ModifiesRelationshipTable::ModifiesRelationshipTable() : RelationshipTable{ 1 } {};

// TODO: replace <int> with <PKBField>
// count of an item in a set can only be 0 or 1
bool ModifiesRelationshipTable::contains(int entity1, int entity2) {
	RelationshipRow r{ entity1, entity2 };
	std::cout << "LOL\n";
	return rows.count(r) == 1;
}

void ModifiesRelationshipTable::insert(int entity1, int entity2) {
	rows.insert(RelationshipRow(entity1, entity2));
}