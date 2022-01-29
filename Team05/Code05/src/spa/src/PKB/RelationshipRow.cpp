#pragma once

#include <stdio.h>
#include <tuple>

#include "RelationshipRow.h"

RelationshipRow::RelationshipRow(int e1, int e2): entity1(e1), entity2(e2) {};

bool RelationshipRow::operator == (const RelationshipRow& row) const {
	return entity1 == row.entity1 && entity2 == row.entity2;
};

// Referenced from https://stackoverflow.com/questions/45966807/c-invalid-comparator-assert
bool RelationshipRow::operator < (const RelationshipRow & row) const {
	//return !(entity1 == row.entity1 && entity2 == row.entity2);
	return std::tie(entity1, entity2) < std::tie(row.entity1, row.entity2);
};
