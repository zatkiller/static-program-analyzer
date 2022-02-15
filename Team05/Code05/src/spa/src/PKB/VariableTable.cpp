#include <stdio.h>
#include <set>

#include "VariableTable.h"

// count of an item in a set can only be 0 or 1
bool VariableTable::contains(std::string name) {
    return rows.count(VariableRow(VAR_NAME{name})) == 1;
}

void VariableTable::insert(std::string name) {
    rows.insert(VariableRow(VAR_NAME{ name }));
}

int VariableTable::getSize() {
    return rows.size();
}

std::vector<VAR_NAME> VariableTable::getAllVars() {
    std::vector<VAR_NAME> res;

    for (const auto& row : rows) {
        res.push_back(row.getVarName());
    }

    return res;
}
