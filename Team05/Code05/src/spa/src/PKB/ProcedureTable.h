#pragma once

#include <stdio.h>
#include <set>

#include "ProcedureRow.h"

class ProcedureTable {
public:
	// count of an item in a set can only be 0 or 1
	bool contains(PROC_NAME);
	void insert(PROC_NAME);
	int getSize();

private:
	std::set<ProcedureRow> rows;
};
