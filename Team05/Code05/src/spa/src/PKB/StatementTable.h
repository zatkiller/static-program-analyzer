#pragma once

#include <stdio.h>
#include <vector>
#include <set>

#include "Table.h"
#include "StatementRow.h"

class StatementTable : Table {
public:
	// TODO: replace <int> with <PKBField>
	bool contains(StatementType, int);
	void insert(StatementType, int);

private:
	std::set<StatementRow> rows;
};