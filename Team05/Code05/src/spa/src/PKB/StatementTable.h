#pragma once

#include <stdio.h>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <iterator>

#include "PKBField.h"
#include "StatementRow.h"

/**
* A data structure to store StatementRows.
*/
class StatementTable {
public:
    /**
    * Checks whether the StatementTable contains the statement information wrapped in a PKBField.
    *
    * @param entry the PKBField containing the statement information to be checked
    * @returns whether the statement information is present in the ConstantTable
    */
    bool contains(StatementType, int);

    bool contains(int);

    /**
    * Inserts statement information wrapped in a PKBField into the StatementTable.
    *
    * @param entry the PKBField containing the statement information to be inserted
    */
    void insert(StatementType, int);

    /**
    * Returns the number of StatementRows.
    *
    * @return number of StatementRows
    */
    int getSize();

    /**
    * Retrieves all statement information stored in the StatementTable.
    *
    * @return a vector of statement information 
    */
    std::vector<STMT_LO> getAllStmt();

    /**
    * Retrieves all statement information of the given type stored in the StatementTable.
    *
    * @param type type of statement
    * @return a vector of statement information belong to the given type
    */
    std::vector<STMT_LO> getStmtOfType(StatementType);

    StatementType getStmtTypeOfLine(int statementNum);

private:
    std::unordered_set<StatementRow, StatementRowHash> rows;
};
