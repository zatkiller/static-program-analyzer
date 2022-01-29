#pragma once

#include <stdio.h>
#include <string>

class ProcedureRow {
public:
	ProcedureRow(std::string);

	bool operator == (const ProcedureRow&) const;
	bool operator < (const ProcedureRow&) const;

private:
	std::string procedureName;
};