#pragma once

#include <stdio.h>
#include <set>

#include "VariableRow.h"

class VariableTable {
public:
	// TODO: replace <int> with <PKBField>
	// count of an item in a set can only be 0 or 1
	bool contains(std::string variableName) {
		return rows.count(VariableRow(variableName)) == 1;
	}

	void insert(std::string variableName) {
		rows.insert(VariableRow(variableName));
	}

private:
	std::set<VariableRow> rows;
};