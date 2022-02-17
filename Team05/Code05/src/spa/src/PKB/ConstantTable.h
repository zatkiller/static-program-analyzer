#pragma once

#include <set>

#include "PKBField.h"
#include "ConstantRow.h"

/**
* A data structure to store ConstantRows.
* 
* @see ConstantRow
*/
class ConstantTable {
public:
    /**
    * Checks whether the ConstantTable contains the constant wrapped in a PKBField.
    * 
    * @param const the constant to be checked
    * @return bool whether constant is present in the ConstantTable
    * @see CONST
    */
    bool contains(CONST constant) const;

    /**
    * Inserts a constant wrapped in a CONST into the ConstantTable.
    *
    * @param entry the constant to be inserted
    * @see CONST
    */
    void insert(CONST constant);

    /**
    * Returns the number of ConstantRows in the ConstantTable.
    * 
    * @return int number of ConstantRows in the ConstantTable
    */
    int getSize() const;

    /**
    * Retrieves a vector of all constants stored in the ConstantTable.
    * 
    * @return std::vector<CONST> a vector of constants
    * @see CONST
    */
     std::vector<CONST> getAllConst() const;

private:
    std::set<ConstantRow> rows;
};
