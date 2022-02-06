#include "PKBDataTypes.h"

bool STMT_LO::operator == (const STMT_LO& other) const {
	return statementNum == other.statementNum && type == other.type;
}

bool STMT_LO::operator < (const STMT_LO& other) const {
	return std::tie(type, statementNum) < std::tie(other.type, other.statementNum);
}

bool VAR_NAME::operator == (const VAR_NAME& other) const {
	return name == other.name;
}

bool VAR_NAME::operator < (const VAR_NAME& other) const {
	return name < other.name;
}

bool PROC_NAME::operator == (const PROC_NAME& other) const {
	return name == other.name;
}

bool PROC_NAME::operator < (const PROC_NAME& other) const {
	return name < other.name;
}