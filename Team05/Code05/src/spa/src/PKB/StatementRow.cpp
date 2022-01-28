#pragma once

#include <stdio.h>

#include "StatementRow.h"

StatementRow::StatementRow(StatementType t, int l) : type(t), lineNumber(l) {}

bool StatementRow::operator == (const StatementRow& row) const {
	return type == row.type && lineNumber == row.lineNumber;
}

bool StatementRow::operator < (const StatementRow& row) const {
	return !(type == row.type && lineNumber == row.lineNumber);
}
