#pragma once

#include <stdio.h>
#include <set>

#include "VariableRow.h"
#include "PKBDataTypes.h"

class VariableTable {
public:
	// count of an item in a set can only be 0 or 1
	bool contains(VAR_NAME);
	void insert(VAR_NAME variableName);
	int getSize();

private:
	std::set<VariableRow> rows;
};
