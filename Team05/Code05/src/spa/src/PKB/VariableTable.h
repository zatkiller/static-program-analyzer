#pragma once

#include <stdio.h>
#include <unordered_set>
#include <vector>

#include "VariableRow.h"
#include "PKBDataTypes.h"
#include "PKBField.h"

class VariableTable {
public:
    // count of an item in a set can only be 0 or 1
    bool contains(std::string name);
    void insert(std::string name);
    int getSize();
    std::vector<VAR_NAME> getAllVars();

private:
    std::unordered_set<VariableRow, VariableRowHash> rows;
};
