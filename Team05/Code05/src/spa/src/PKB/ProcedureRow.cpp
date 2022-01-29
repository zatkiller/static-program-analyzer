#pragma once

#include <stdio.h>

#include "ProcedureRow.h"

ProcedureRow::ProcedureRow(std::string p) : procedureName(p) {}

bool ProcedureRow::operator == (const ProcedureRow& row) const {
	return procedureName == row.procedureName;
}

bool ProcedureRow::operator < (const ProcedureRow& row) const {
	return !(procedureName == row.procedureName);
}