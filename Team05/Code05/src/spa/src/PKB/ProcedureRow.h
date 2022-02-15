#pragma once

#include <stdio.h>
#include <string>

#include "PKBDataTypes.h"

/**
* A data structure to store procedure names (strings) in a ProcedureTable.
*/
class ProcedureRow {
public:
    explicit ProcedureRow(PROC_NAME);
    
    /**
    * Retrieves the procedure name stored in the ProcedureRow.
    *
    * @return a procedure name wrapped in PROC_NAME
    */
    PROC_NAME getProcName() const;
    bool operator == (const ProcedureRow&) const;
    bool operator < (const ProcedureRow& other) const;

private:
    PROC_NAME procedureName;
};

/**
* Hash function for ProcedureRow.
*/
class ProcedureRowHash {
    public:
        size_t operator() (const ProcedureRow&) const;
};
