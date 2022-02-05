#pragma once

#include <stdio.h>
#include <set>

#include "PKBField.h"
#include "StatementRow.h"

class StatementTable {
public:
	bool contains(StatementType, int);
	void insert(StatementType, int);
	int getSize();

private:
	std::set<StatementRow> rows;
};
