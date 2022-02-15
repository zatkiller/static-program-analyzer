#pragma once

#include <stdio.h>
#include <set>
#include <vector>

#include "VariableRow.h"
#include "PKBDataTypes.h"
#include "PKBField.h"

/**
* A data structure to store VariableRows.
*/
class VariableTable {
public:
    /**
    * Checks whether the VariableTable contains the variable name wrapped in a PKBField.
    *
    * @param name the PKBField containing the variable name to be checked
    * @returns whether the variable name is present in the VariableTable
    */        
    bool contains(std::string name);

    /**
    * Inserts a variable name wrapped in a PKBField into the VariableTable.
    *
    * @param name the PKBField containing the variable name to be inserted
    */
    void insert(std::string name);

    /**
    * Returns the number of VariableRows.
    *
    * @return number of VariableRows
    */
    int getSize();

    /**
    * Retrieves all variable names stored in the VariableTable.
    *
    * @return a vector of variable names
    */
    std::vector<VAR_NAME> getAllVars();

private:
    std::set<VariableRow> rows;
};
