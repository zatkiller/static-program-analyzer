#pragma once

#include <stdio.h>
#include <set>

#include "StatementRow.h"

class StatementTable {
public:
	// TODO: replace <int> with <PKBField>
	bool contains(StatementType, int);
	void insert(StatementType, int);
	int getSize();

private:
	std::set<StatementRow> rows;
};
