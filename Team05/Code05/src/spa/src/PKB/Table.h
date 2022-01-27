#pragma once

#include <stdio.h>
#include <vector>
#include <set>

#include "Row.h"

class Table {
public:
	// TODO: replace <int> with <PKBField>
	virtual bool contains(std::vector<int> queries) = 0; 
	virtual bool insert(std::vector<int> params) = 0;
	virtual ~Table();
};