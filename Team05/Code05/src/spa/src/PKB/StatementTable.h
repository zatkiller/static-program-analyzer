#pragma once

#include <stdio.h>
#include <unordered_set>
#include <vector>

#include "PKBField.h"
#include "StatementRow.h"

class StatementTable {
public:
	bool contains(StatementType, int);
	void insert(StatementType, int);
	int getSize();
	std::vector<STMT_LO> getAllStmt();
	std::vector<STMT_LO> getStmtOfType(StatementType);

private:
	std::unordered_set<StatementRow, StatementRowHash> rows;
};
