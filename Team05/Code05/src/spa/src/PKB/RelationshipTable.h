#pragma once

#include <stdio.h>
#include <set>

#include "RelationshipRow.h"

class RelationshipTable {
public:
	// TODO: replace <int> with <PKBField>
	// count of an item in a set can only be 0 or 1
	RelationshipTable(int t);
	virtual bool contains(int entity1, int entity2) = 0;
	virtual void insert(int entity1, int entity2) = 0;
	int getType();

protected:
	std::set<RelationshipRow> rows;
	int type; // TODO: replace int with PKBRelationshipType
};

