#pragma once

#include "PKBType.h"
#include "PKBDataTypes.h"

struct PKBField {
    PKBType tag;
    bool isConcrete;
    union Content {
        STMT_LO stmtNum;
        VAR_NAME varName;
        PROC_NAME procName;
        CONST constName;
    };

    public:
        bool operator == (const PKBField&) const;
};
