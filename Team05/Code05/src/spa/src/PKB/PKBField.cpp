#pragma once

#include "PKBField.h"
#include "PKBDataTypes.h"

bool PKBField::operator == (const PKBField& other) const {
	if (tag == other.tag && isConcrete == other.isConcrete) {
		return content == other.content;
	}
	return false;
}

bool PKBField::operator < (const PKBField& other) const {
	return std::tie(tag, isConcrete, content) < std::tie(other.tag, other.isConcrete, other.content);

	/*if (tag == other.tag && isConcrete == other.isConcrete) {
		PKBType type = tag;
		switch (type) {
		case PKBType::STATEMENT:
			return std::get<STMT_LO>(content) < std::get<STMT_LO>(other.content);
		case PKBType::VARIABLE:
			return std::get<VAR_NAME>(content) < std::get<VAR_NAME>(other.content);
		case PKBType::PROCEDURE:
			return std::get<PROC_NAME>(content) < std::get<PROC_NAME>(other.content);
		case PKBType::CONST:
			return std::get<CONST>(content) < std::get<CONST>(other.content);
		default:
			break;
		}
	}
	return false;*/
}