#pragma once

#include <stdio.h>
#include <set>

#include "VariableTable.h"

// count of an item in a set can only be 0 or 1
bool VariableTable::contains(PKBField field) {
	if (field.tag != PKBType::VARIABLE) {
		throw "Only variables are accepted!";
	}
	VAR_NAME var = std::get<VAR_NAME>(field.content);
	return rows.count(VariableRow(var)) == 1;
}

void VariableTable::insert(PKBField field) {
	if (field.tag != PKBType::VARIABLE) {
		throw "Only variables are accepted!";
	}
	VAR_NAME var = std::get<VAR_NAME>(field.content);
	rows.insert(VariableRow(var));
}

int VariableTable::getSize() {
	return rows.size();
}
