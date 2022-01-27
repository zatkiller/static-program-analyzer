#pragma once

#include <stdio.h>
#include <vector>
#include <set>

#include "../logging.h"
#include "Row.h"
#include "StatementType.h"

class StatementRow : Row {
public:
	StatementRow(StatementType, int);

	bool equal(const StatementRow&);
	bool operator == (const StatementRow&) const;
	bool operator < (const StatementRow&) const;

private:
	StatementType type;
	int lineNumber;
};