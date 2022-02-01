#pragma once

#include <stdio.h>
#include <tuple>

#include "PKBDataTypes.h"
#include "ProcedureRow.h"

ProcedureRow::ProcedureRow(PROC_NAME p) : procedureName(p) {}

bool ProcedureRow::operator == (const ProcedureRow& row) const {
	return procedureName == row.procedureName;
}

bool ProcedureRow::operator < (const ProcedureRow& row) const {
	return std::tie(procedureName) < std::tie(row.procedureName);
}
