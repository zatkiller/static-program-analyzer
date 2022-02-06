#include <stdio.h>
#include <tuple>

#include "PKBDataTypes.h"
#include "ProcedureRow.h"

ProcedureRow::ProcedureRow(PROC_NAME p) : procedureName(p) {}

bool ProcedureRow::operator == (const ProcedureRow& row) const {
	return procedureName == row.procedureName;
}

PROC_NAME ProcedureRow::getProcName() const {
	return procedureName;
}

size_t ProcedureRowHash::operator() (const ProcedureRow& other) const {
	return std::hash<std::string>()(other.getProcName().name);
}