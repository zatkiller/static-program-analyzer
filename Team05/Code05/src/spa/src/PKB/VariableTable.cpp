#pragma once

#include <stdio.h>
#include <set>

#include "VariableTable.h"

// TODO: replace <int> with <PKBField>
// count of an item in a set can only be 0 or 1
bool VariableTable::contains(std::string variableName) {
	return rows.count(VariableRow(variableName)) == 1;
}

void VariableTable::insert(std::string variableName) {
	rows.insert(VariableRow(variableName));
}

int VariableTable::getSize() {
	return rows.size();
}