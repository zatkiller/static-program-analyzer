#pragma once

#include <stdio.h>
#include <string>

class VariableRow {
public:
	VariableRow(std::string);

	bool operator == (const VariableRow&) const;
	bool operator < (const VariableRow&) const;

private:
	std::string variableName;
};
