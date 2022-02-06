#pragma once

#include <stdio.h>
#include <string>

#include "PKBDataTypes.h"

class ProcedureRow {
public:
    explicit ProcedureRow(PROC_NAME);

    bool operator == (const ProcedureRow&) const;
    PROC_NAME getProcName() const;

private:
    PROC_NAME procedureName;
};

class ProcedureRowHash {
    public:
        size_t operator() (const ProcedureRow&) const;
};
