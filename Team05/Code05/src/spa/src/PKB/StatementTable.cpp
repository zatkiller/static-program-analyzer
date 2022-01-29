#pragma once

#include <stdio.h>

#include "StatementTable.h"

// TODO: replace <int> with <PKBField>
// count of an item in a set can only be 0 or 1
bool StatementTable::contains(StatementType type, int lineNumber) {
	return rows.count(StatementRow(type, lineNumber)) == 1;
}

void StatementTable::insert(StatementType type, int lineNumber) {
	rows.insert(StatementRow(type, lineNumber));
}

int StatementTable::getSize() {
	return rows.size();
}