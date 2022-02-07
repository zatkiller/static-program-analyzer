#pragma once

#include <unordered_set>

#include "PKBField.h"
#include "ConstantRow.h"

/**
* A data structure to store ConstantRows.
*/
class ConstantTable {
public:
    /**
    * Checks whether the ConstantTable contains a constant wrapped in a PKBField.
    * 
    * @param entry the PKBField containing the constant to be checked
    * @returns whether the constant is present in the ConstantTable
    */
    bool contains(PKBField entry);

    /**
    * Inserts a constant wrapped in a PKBField into the ConstantTable.
    *
    * @param entry the PKBField containing the constant to be inserted
    */
    void insert(PKBField entry);

    /**
    * Returns the number of ConstantRows.
    * 
    * @return number of ConstantRows
    */
    int getSize();

    /**
    * Retrieves all constants stored in the ConstantTable.
    * 
    * @return a vector of constants
    */
     std::vector<CONST> getAllConst();

private:
    std::unordered_set<ConstantRow, ConstantRowHash> rows;
};
