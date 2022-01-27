#pragma once

#include <stdio.h>
#include <vector>
#include <set>
#include <utility>

class Row {
public:
	// TODO: replace <int,int> with <PKBType, String>
	virtual bool contains(std::vector<std::pair<int, int>> params) = 0; // each pair is a (column, value)
	virtual ~Row() {}
};