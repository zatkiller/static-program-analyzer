#include <stdio.h>
#include <string>
#include <tuple>

#include "VariableRow.h"

VariableRow::VariableRow(VAR_NAME v) : variableName(v) {}

bool VariableRow::operator == (const VariableRow& row) const {
	return variableName == row.variableName;
}

VAR_NAME VariableRow::getVarName() const {
	return variableName;
}

size_t VariableRowHash::operator()(const VariableRow& varRow) const {
	std::string varName = varRow.getVarName().name;
	return std::hash<std::string>()(varName);
}
