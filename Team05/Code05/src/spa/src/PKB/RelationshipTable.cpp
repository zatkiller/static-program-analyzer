#pragma once

#include <stdio.h>

#include "RelationshipTable.h"

RelationshipTable::RelationshipTable(int t) : type(t) {}

int RelationshipTable::getType() {
	return type;
}