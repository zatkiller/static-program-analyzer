#pragma once

#include <stdio.h>
#include <string>

#include "PKBDataTypes.h"

class VariableRow {
public:
	VariableRow(VAR_NAME);

	bool operator == (const VariableRow&) const;
	bool operator < (const VariableRow&) const;

private:
	VAR_NAME variableName;
};
