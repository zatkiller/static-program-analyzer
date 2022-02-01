#pragma once

#include <stdio.h>
#include <string>

#include "PKBDataTypes.h"

class ProcedureRow {
public:
	ProcedureRow(PROC_NAME);

	bool operator == (const ProcedureRow&) const;
	bool operator < (const ProcedureRow&) const;

private:
	PROC_NAME procedureName;
};
