#pragma once

#include <stdio.h>
#include <set>

#include "StatementRow.h"

class StatementTable {
public:
	// TODO: replace <int> with <PKBField>
	// count of an item in a set can only be 0 or 1
	bool contains(StatementType type, int lineNumber) {
		return rows.count(StatementRow(type, lineNumber)) == 1;
	}

	void insert(StatementType type, int lineNumber) {
		rows.insert(StatementRow(type, lineNumber));
	}

private:
	std::set<StatementRow> rows;
};