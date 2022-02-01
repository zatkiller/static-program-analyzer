#pragma once

#include <stdio.h>
#include <tuple>

#include "StatementRow.h"

StatementRow::StatementRow(StatementType t, int stmtNum) : stmt{stmtNum, t} {}

bool StatementRow::operator == (const StatementRow& row) const {
	return stmt == row.stmt;
}

bool StatementRow::operator < (const StatementRow& row) const {
	return stmt < row.stmt;
}
