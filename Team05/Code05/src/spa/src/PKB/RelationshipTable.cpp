#pragma once

#include <stdio.h>

#include "RelationshipTable.h"

RelationshipTable::RelationshipTable(PKBRelationship rsType) : type(rsType) {}

PKBRelationship RelationshipTable::getType() {
	return type;
}

int RelationshipTable::getSize() {
	return rows.size();
}
