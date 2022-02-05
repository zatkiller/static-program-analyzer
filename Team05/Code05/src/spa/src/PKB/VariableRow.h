#pragma once

#include <stdio.h>
#include <string>

#include "PKBDataTypes.h"

class VariableRow {
public:
	VariableRow(VAR_NAME);

	bool operator == (const VariableRow&) const;
	VAR_NAME getVarName() const;

private:
	VAR_NAME variableName;
};

class VariableRowHash {
	public:
		size_t operator()(const VariableRow&) const;
};