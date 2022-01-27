#pragma once

#include <stdio.h>
#include <vector>
#include <set>

#include "../logging.h"
#include "StatementRow.h"

class StatementTable {
public:
	// TODO: replace <int> with <PKBField>
	bool contains(StatementType type, int lineNumber) {
		return rows.count(StatementRow(type, lineNumber)) == 1;
	}

	void insert(StatementType type, int lineNumber) {
		rows.insert(StatementRow(type, lineNumber));
	}

private:
	std::set<StatementRow> rows;
};