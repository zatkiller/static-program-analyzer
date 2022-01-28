#pragma once

#include <stdio.h>
#include <set>

#include "RelationshipTable.h"

class ModifiesRelationshipTable : RelationshipTable {
public:
	ModifiesRelationshipTable(int);

	// TODO: replace <int> with <PKBField>
	// count of an item in a set can only be 0 or 1
	bool contains(int entity1, int entity2);
	void insert(int entity1, int entity2);
};