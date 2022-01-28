#pragma once

#include <stdio.h>

#include "StatementType.h"

class StatementRow {
public:
	StatementRow(StatementType, int);

	bool operator == (const StatementRow&) const;
	bool operator < (const StatementRow&) const;

private:
	StatementType type;
	int lineNumber;
};