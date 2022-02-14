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
    * Checks whether the ConstantTable contains the constant wrapped in a PKBField.
    * 
    * @param const the constant to be checked
    * @returns whether the constant is present in the ConstantTable
    */
    bool contains(CONST constant) const;

    /**
    * Inserts a constant wrapped in a PKBField into the ConstantTable.
    *
    * @param entry the constant to be inserted
    */
    void insert(CONST constant);

    /**
    * Returns the number of ConstantRows.
    * 
    * @return number of ConstantRows
    */
    int getSize() const;

    /**
    * Retrieves all constants stored in the ConstantTable.
    * 
    * @return a vector of constants
    */
     std::vector<CONST> getAllConst() const;

private:
    std::unordered_set<ConstantRow, ConstantRowHash> rows;
};
