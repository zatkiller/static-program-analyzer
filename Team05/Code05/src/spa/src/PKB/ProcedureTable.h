#pragma once

#include <stdio.h>
#include <set>

#include "ProcedureRow.h"
#include "PKBField.h"

/**
* A data structure to store ProcedureRows.
*/
class ProcedureTable {
public:
    /**
    * Checks whether the ProcedureTable contains the procedure name.
    *
    * @param name  
    * @returns whether the procedure name is present in the ProcedureTable
    */    
    bool contains(std::string name);

    /**
    * Inserts a procedure name into the ProcedureTable.
    *
    * @param name  
    */
    void insert(std::string name);

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
    std::set<ProcedureRow> rows;
};
