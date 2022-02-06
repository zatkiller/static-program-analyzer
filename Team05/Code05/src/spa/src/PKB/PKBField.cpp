#pragma once

#include <vector>

#include "PKBField.h"
#include "PKBDataTypes.h"

bool PKBField::operator == (const PKBField& other) const {
	if (tag == other.tag && isConcrete == other.isConcrete) {
		return content == other.content;
	}
	return false;
}

size_t PKBFieldHash::operator() (const PKBField& other) const {
	PKBType type = other.tag;

	if (other.isConcrete) {
		switch (type)
		{
		case PKBType::STATEMENT:
			return std::hash<int>()(std::get<STMT_LO>(other.content).statementNum);
		case PKBType::VARIABLE:
			return std::hash<std::string>()(std::get<VAR_NAME>(other.content).name);
		case PKBType::PROCEDURE:
			return std::hash<std::string>()(std::get<PROC_NAME>(other.content).name);
		case PKBType::CONST:
			return std::hash<int>()(std::get<CONST>(other.content));
		default:
			break;
		}
	}
	else {
		return std::hash<PKBType>()(other.tag);
	}
}

size_t PKBFieldVectorHash::operator() (const std::vector<PKBField>& other) const {
	return std::hash<int>()(other.size());
}
