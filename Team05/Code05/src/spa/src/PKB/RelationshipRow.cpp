#pragma once

#include <stdio.h>

class RelationshipRow {
public:
	RelationshipRow(int e1, int e2): entity1(e1), entity2(e2) {};

	bool operator == (const RelationshipRow& row) const {
		return entity1 == row.entity1 && entity2 == row.entity2;
	};

	bool operator < (const RelationshipRow& row) const {
		return !(entity1 == row.entity1 && entity2 == row.entity2);
	};

private:
	// TODO: change to PKBField
	int entity1;
	int entity2;
};