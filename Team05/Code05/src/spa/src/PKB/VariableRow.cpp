#pragma once

#include <stdio.h>
#include <string>

#include "VariableRow.h"

VariableRow::VariableRow(std::string v) : variableName(v) {}

bool VariableRow::operator == (const VariableRow& row) const {
	return variableName == row.variableName;
}

bool VariableRow::operator < (const VariableRow& row) const {
	return !(variableName == row.variableName);
}