#pragma once

#include <stdio.h>
#include <set>

#include "VariableRow.h"
#include "PKBDataTypes.h"
#include "PKBField.h"

class VariableTable {
public:
	// count of an item in a set can only be 0 or 1
	bool contains(PKBField);
	void insert(PKBField);
	int getSize();

private:
	std::set<VariableRow> rows;
};
