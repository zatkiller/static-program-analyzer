#pragma once

#include <stdio.h>
#include <set>

#include "RelationshipTable.h"


RelationshipTable::RelationshipTable(int t) : type(t) {}
int RelationshipTable::getType() {
	return type;
}