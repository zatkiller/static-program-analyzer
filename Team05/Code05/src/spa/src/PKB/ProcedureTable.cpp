#pragma once

#include <stdio.h>
#include <set>

#include "ProcedureTable.h"

// count of an item in a set can only be 0 or 1
bool ProcedureTable::contains(PROC_NAME procedureName) {
	return rows.count(ProcedureRow(procedureName)) == 1;
}

void ProcedureTable::insert(PROC_NAME procedureName) {
	rows.insert(ProcedureRow(procedureName));
}

int ProcedureTable::getSize() {
	return rows.size();
}
