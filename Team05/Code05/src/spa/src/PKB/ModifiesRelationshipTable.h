#pragma once

#include <stdio.h>
#include <set>

#include "RelationshipTable.h"

class ModifiesRelationshipTable : public RelationshipTable {
public:
	ModifiesRelationshipTable();

	// TODO: replace <int> with <PKBField>
	// count of an item in a set can only be 0 or 1
	bool contains(int, int);
	void insert(int, int);
};
