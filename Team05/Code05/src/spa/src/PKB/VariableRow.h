#pragma once

#include <stdio.h>
#include <string>

#include "PKBDataTypes.h"

/**
* A data structure to store variable names (strings) in a VariableTable.
*/
class VariableRow {
public:
    explicit VariableRow(VAR_NAME);

    /**
    * Retrieves the variable name stored in the VariableTable.
    *
    * @return a variable name wrapped in VAR_NAME
    */
    VAR_NAME getVarName() const;
    bool operator == (const VariableRow&) const;

private:
    VAR_NAME variableName;
};

/**
* Hash function for VariableTable.
*/
class VariableRowHash {
    public:
        size_t operator()(const VariableRow&) const;
};
