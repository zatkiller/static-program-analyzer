#pragma once

#include <stdio.h>
#include <vector>
#include <set>

#include "../logging.h"
#include "Row.h"
#include "StatementType.h"

class StatementRow: Row {
public:
	StatementRow(StatementType t, int l);

	bool equal(const StatementRow& row);
	bool operator == (const StatementRow& row);
	bool operator < (const StatementRow& row);

private:
	StatementType type;
	int lineNumber;
};