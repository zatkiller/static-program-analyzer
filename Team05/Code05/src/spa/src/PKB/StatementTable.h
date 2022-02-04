#pragma once

#include <stdio.h>
#include <set>

#include "PKBField.h"
#include "StatementRow.h"

class StatementTable {
public:
	bool contains(PKBField);
	void insert(PKBField);
	int getSize();

private:
	std::set<StatementRow> rows;
};
