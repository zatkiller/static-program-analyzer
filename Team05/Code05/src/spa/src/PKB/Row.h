#pragma once

#include <stdio.h>
#include <vector>
#include <set>

class Row {
public:
	// TODO: replace <int> withh <PKBField>
	virtual bool contains(std::vector<int> queries) = 0; 
	virtual ~Row() {}
};