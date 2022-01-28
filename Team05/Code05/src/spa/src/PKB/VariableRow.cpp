#pragma once

#include <stdio.h>
#include <string>

class VariableRow {
public:
	VariableRow(std::string v): variableName(v) {}

	bool operator == (const VariableRow& row) const {
		return variableName == row.variableName;
	}

	bool operator < (const VariableRow& row) const {
		return !(variableName == row.variableName);
	}

private:
	std::string variableName;
};