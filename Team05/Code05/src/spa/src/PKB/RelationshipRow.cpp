#pragma once

#include <stdio.h>

#include "RelationshipRow.h"

RelationshipRow::RelationshipRow(int e1, int e2): entity1(e1), entity2(e2) {};

bool RelationshipRow::operator == (const RelationshipRow& row) const {
	return entity1 == row.entity1 && entity2 == row.entity2;
};

bool RelationshipRow::operator < (const RelationshipRow & row) const {
	return !(entity1 == row.entity1 && entity2 == row.entity2);
};
