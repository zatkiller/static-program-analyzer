#pragma once

#include <stdio.h>
#include <unordered_set>

#include "PKBField.h"
#include "StatementRow.h"

class StatementTable {
public:
	bool contains(PKBField);
	void insert(PKBField);
	int getSize();

private:
	std::unordered_set<StatementRow, StatementRowHash> rows;
};
