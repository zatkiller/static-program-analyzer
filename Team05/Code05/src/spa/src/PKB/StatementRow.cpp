#pragma once

#include <stdio.h>

#include "StatementType.h"

class StatementRow {
public:
	StatementRow(StatementType t, int l) : type(t), lineNumber(l) {}

	bool operator == (const StatementRow& row) const {
		return type == row.type && lineNumber == row.lineNumber;
	}

	bool operator < (const StatementRow& row) const {
		return !(type == row.type && lineNumber == row.lineNumber);
	}

private:
	StatementType type;
	int lineNumber;
};