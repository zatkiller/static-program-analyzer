#pragma once

#include <stdio.h>
#include <set>

#include "VariableTable.h"

// count of an item in a set can only be 0 or 1
bool VariableTable::contains(VAR_NAME variableName) {
	return rows.count(VariableRow(variableName)) == 1;
}

void VariableTable::insert(VAR_NAME variableName) {
	rows.insert(VariableRow(variableName));
}

int VariableTable::getSize() {
	return rows.size();
}
