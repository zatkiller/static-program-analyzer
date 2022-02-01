#pragma once

#include <stdio.h>

#include "StatementTable.h"

// TODO: replace <int> with <PKBField>
// count of an item in a set can only be 0 or 1
bool StatementTable::contains(STMT_LO stmt) {
	return rows.count(StatementRow(stmt.type, stmt.statementNum)) == 1;
}

void StatementTable::insert(STMT_LO stmt) {
	rows.insert(StatementRow(stmt.type, stmt.statementNum));
}

int StatementTable::getSize() {
	return rows.size();
}
