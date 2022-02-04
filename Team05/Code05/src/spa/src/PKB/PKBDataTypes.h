#pragma once

#include <string>
#include "StatementType.h"

typedef int CONST;

typedef struct VAR_NAME {
    VAR_NAME(std::string str) : name(str) {};
    std::string name;
} VAR_NAME;

typedef struct PROC_NAME {
    PROC_NAME(std::string str) : name(str) {};
    std::string name;
} PROC_NAME;

typedef struct STMT_LO {
    int statementNum;
    StatementType type;

    public:
        bool operator == (const STMT_LO&) const;
        bool operator < (const STMT_LO&) const;
} STMT_LO;
