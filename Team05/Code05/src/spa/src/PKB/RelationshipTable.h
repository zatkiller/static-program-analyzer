#pragma once

#include <stdio.h>
#include <set>

#include "RelationshipRow.h"
#include "PKBRelationship.h"

class RelationshipTable {
public:
	// count of an item in a set can only be 0 or 1
	RelationshipTable(PKBRelationship);

	virtual bool contains(int, int) = 0;
	virtual void insert(int, int) = 0;
	PKBRelationship getType();
	int getSize();

protected:
	std::set<RelationshipRow> rows;
	PKBRelationship type; 
};
