#pragma once

#include <string>
#include <tuple>
#include <optional>
#include "StatementType.h"

typedef int CONST;

typedef struct VAR_NAME {
    VAR_NAME(std::string str) : name(str) {}
    std::string name;

    bool operator == (const VAR_NAME&) const;
    bool operator < (const VAR_NAME&) const;
} VAR_NAME;

typedef struct PROC_NAME {
    PROC_NAME(std::string str) : name(str) {}
    std::string name;

    bool operator == (const PROC_NAME&) const;
    bool operator < (const PROC_NAME&) const;
} PROC_NAME;

typedef struct STMT_LO {
    STMT_LO(int statementNum) : statementNum(statementNum), type(std::nullopt) {}
    STMT_LO(int statementNum, StatementType type) : statementNum(statementNum), type(type) {}
    int statementNum;
    std::optional<StatementType> type;

    bool hasStatementType();
    bool operator == (const STMT_LO&) const;
    bool operator < (const STMT_LO&) const;
} STMT_LO;
