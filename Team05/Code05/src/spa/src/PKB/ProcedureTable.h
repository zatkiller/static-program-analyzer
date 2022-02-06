#pragma once

#include <stdio.h>
#include <unordered_set>

#include "ProcedureRow.h"
#include "PKBField.h"

class ProcedureTable {
public:
	// count of an item in a set can only be 0 or 1
	bool contains(PKBField);
	void insert(PKBField);
	int getSize();
	std::vector<PROC_NAME> getAllProcs();

private:
	std::unordered_set<ProcedureRow, ProcedureRowHash> rows;
};
