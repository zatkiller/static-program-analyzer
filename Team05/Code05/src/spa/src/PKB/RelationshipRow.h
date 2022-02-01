#pragma once

#include <stdio.h>

#include "PKBField.h"

class RelationshipRow {
public:
	RelationshipRow(PKBField, PKBField);

	bool operator == (const RelationshipRow&) const;
	bool operator < (const RelationshipRow&) const;

private:
	PKBField entity1;
	PKBField entity2;
};
