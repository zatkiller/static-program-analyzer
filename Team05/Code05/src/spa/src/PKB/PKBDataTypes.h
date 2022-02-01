#pragma once

#include <string>
#include "StatementType.h"

typedef std::string VAR_NAME;
typedef std::string PROC_NAME;
typedef int CONST;
typedef struct STMT_LO {
    int statementNum;
    StatementType type;

    public:
        bool operator == (const STMT_LO&) const;
        bool operator < (const STMT_LO&) const;
} STMT_LO;
