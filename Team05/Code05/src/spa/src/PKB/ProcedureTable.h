#pragma once

#include <stdio.h>
#include <unordered_set>

#include "ProcedureRow.h"
#include "PKBField.h"

/**
* A data structure to store ProcedureRows.
*/
class ProcedureTable {
public:
    /**
    * Checks whether the ProcedureTable contains the procedure name wrapped in a PKBField.
    *
    * @param field the PKBField containing the procedure name to be checked
    * @returns whether the procedure name is present in the ProcedureTable
    */    
    bool contains(PKBField field);

    /**
    * Inserts a procedure name wrapped in a PKBField into the ProcedureTable.
    *
    * @param field the PKBField containing the procedure name to be inserted
    */
    void insert(PKBField field);

    /**
    * Returns the number of ProcedureRows.
    *
    * @return number of ProcedureRows
    */
    int getSize();

    /**
    * Retrieves all procedure names stored in the ProcedureTable.
    *
    * @return a vector of procedure names
    */
    std::vector<PROC_NAME> getAllProcs();

private:
    std::unordered_set<ProcedureRow, ProcedureRowHash> rows;
};
