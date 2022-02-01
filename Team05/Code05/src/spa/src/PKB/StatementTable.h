#pragma once

#include <stdio.h>
#include <set>

#include "StatementRow.h"

class StatementTable {
public:
	// TODO: replace <int> with <PKBField>
	bool contains(STMT_LO stmt);
	void insert(STMT_LO stmt);
	int getSize();

private:
	std::set<StatementRow> rows;
};
