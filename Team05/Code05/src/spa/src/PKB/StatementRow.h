#pragma once

#include <stdio.h>

#include "StatementType.h"
#include "PKBDataTypes.h"

/**
* A data structure to store statement information (line number and statement type) in a StatementTable.
*/
class StatementRow {
public:
    StatementRow(StatementType, int);

    /**
    * Retrieves the statement information stored in the StatementRow.
    *
    * @return the statement information wrapped in CONST
    */
    STMT_LO getStmt() const;
    bool operator == (const StatementRow&) const;

private:
    STMT_LO stmt;
};

/**
* Hash function for StatementRow.
*/
class StatementRowHash {
    public:
        size_t operator() (const StatementRow& other) const;
};
