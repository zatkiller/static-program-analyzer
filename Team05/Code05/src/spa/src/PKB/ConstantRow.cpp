#pragma once

#include "ConstantRow.h"

bool ConstantRow::operator == (const ConstantRow& other) const {
	return constant == other.constant;
}

CONST ConstantRow::getConst() const {
	return constant;
}

size_t ConstantRowHash::operator() (const ConstantRow& other) const {
	return std::hash<CONST>()(other.getConst());
}
