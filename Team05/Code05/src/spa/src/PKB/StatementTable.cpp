#include <stdio.h>
#include "StatementTable.h"
#include "PKBField.h"

bool StatementTable::contains(StatementType type, int statementNumber) {
    return rows.count(StatementRow(type, statementNumber)) == 1;
}

bool StatementTable::contains(int statementNumber) {
    return contains(StatementType::Assignment, statementNumber) ||
        contains(StatementType::Call, statementNumber) ||
        contains(StatementType::If, statementNumber) ||
        contains(StatementType::Read, statementNumber) ||
        contains(StatementType::Print, statementNumber);
}

void StatementTable::insert(StatementType type, int statementNumber) {
    // table can't have 2 rows with the same statement number
    if (!contains(statementNumber)) {
        rows.insert(StatementRow(type, statementNumber));
    }
}

int StatementTable::getSize() {
    return rows.size();
}

std::vector<STMT_LO> StatementTable::getAllStmt() {
    std::vector<STMT_LO> res;

    for (auto iter = rows.begin(); iter != rows.end(); ++iter) {
        res.push_back(iter->getStmt());
    }

    return res;
}

std::vector<STMT_LO> StatementTable::getStmtOfType(StatementType type) {
    std::vector<STMT_LO> res;

    for (auto iter = rows.begin(); iter != rows.end(); ++iter) {
        if (iter->getStmt().type == type) {
            res.push_back(iter->getStmt());
        }
    }

    return res;
}

StatementType StatementTable::getStmtTypeOfLine(int statementNum) {
    for (auto iter = rows.begin(); iter != rows.end(); ++iter) {
        STMT_LO stmt = iter->getStmt();
        if (stmt.statementNum == statementNum && stmt.hasStatementType()) {
            return stmt.type.value();
        }
    }

    throw "No statement exists with the provided statement number";
}
