#pragma once

#include <stdio.h>

#include "StatementType.h"
#include "PKBDataTypes.h"

class StatementRow {
public:
	StatementRow(StatementType, int);

	bool operator == (const StatementRow&) const;
	bool operator < (const StatementRow&) const;

private:
	STMT_LO stmt;
};
