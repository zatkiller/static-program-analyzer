#pragma once

#include "PKBDataTypes.h"

bool STMT_LO::operator == (const STMT_LO& other) const {
	return statementNum == other.statementNum && type == other.type;
}

bool STMT_LO::operator < (const STMT_LO& other) const {
	return std::tie(type, statementNum) < std::tie(other.type, other.statementNum);
}
