#pragma once

#include <stdio.h>

#include "StatementType.h"
#include "PKBDataTypes.h"

class StatementRow {
public:
	StatementRow(StatementType, int);

	bool operator == (const StatementRow&) const;
	STMT_LO getStmt() const;

private:
	STMT_LO stmt;
};

class StatementRowHash {
	public:
		size_t operator() (const StatementRow& other) const;
};
