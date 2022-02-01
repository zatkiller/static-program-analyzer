#pragma once

#include <stdio.h>

class RelationshipRow {
public:
	RelationshipRow(int, int);

	bool operator == (const RelationshipRow&) const;
	bool operator < (const RelationshipRow&) const;

private:
	// TODO: change to PKBField
	int entity1;
	int entity2;
};
