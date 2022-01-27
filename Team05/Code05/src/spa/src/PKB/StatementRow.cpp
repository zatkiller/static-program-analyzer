#pragma once

#include <stdio.h>
#include <vector>
#include <set>

#include "../logging.h"
#include "Row.h"
#include "StatementType.h"

class StatementRow : Row {
public:
	StatementRow(StatementType t, int l) : type(t), lineNumber(l) {}

	bool equal(const StatementRow& row) {
		if (type != row.type || lineNumber != row.lineNumber) {
			return false;
		}

		return true;
	}

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