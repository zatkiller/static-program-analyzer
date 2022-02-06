#include <stdio.h>
#include <tuple>

#include "StatementRow.h"

StatementRow::StatementRow(StatementType t, int stmtNum) : stmt{stmtNum, t} {}

bool StatementRow::operator == (const StatementRow& row) const {
	return stmt == row.stmt;
}

STMT_LO StatementRow::getStmt() const {
	return stmt;
}

size_t StatementRowHash::operator() (const StatementRow& other) const {
	return std::hash<int>()(other.getStmt().statementNum);
}
