#pragma once

#include <unordered_set>

#include "PKBField.h"
#include "ConstantRow.h"

class ConstantTable {
public:
	bool contains(PKBField);
	void insert(PKBField);
	int getSize();
	std::vector<CONST> getAllConst();

private:
	std::unordered_set<ConstantRow, ConstantRowHash> rows;
};
