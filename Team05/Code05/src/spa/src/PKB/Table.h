#pragma once

#include <stdio.h>
#include <vector>
#include <set>
#include <utility>

#include "Row.h"

class Table {
public:
	// TODO: replace <int,int> with <PKBType, String>
	// perhaps can create a union for String and int (PKBValue)?
	virtual bool contains(std::vector<std::pair<int, int>> params) = 0; // each pair is a (column, value)
	virtual bool insert(std::vector<std::pair<int, int>> params) = 0;
	virtual ~Table();

private:
	std::set<Row> rows;
};