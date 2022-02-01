#pragma once

#include <stdio.h>
#include <set>

#include "ProcedureTable.h"

// TODO: replace <int> with <PKBField>
// count of an item in a set can only be 0 or 1
bool ProcedureTable::contains(std::string procedureName) {
	return rows.count(ProcedureRow(procedureName)) == 1;
}

void ProcedureTable::insert(std::string procedureName) {
	rows.insert(ProcedureRow(procedureName));
}

int ProcedureTable::getSize() {
	return rows.size();
}
