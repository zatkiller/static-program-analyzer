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
        contains(StatementType::Print, statementNumber) ||
        contains(StatementType::While, statementNumber);
}

void StatementTable::insert(StatementType type, int statementNumber) {
    if (!contains(statementNumber)) {
        rows.insert(StatementRow(type, statementNumber));
    }
}

int StatementTable::getSize() {
    return rows.size();
}

std::vector<STMT_LO> StatementTable::getAllStmt() {
    std::vector<STMT_LO> res;

    for (const auto& row : rows) {
        res.push_back(row.getStmt());
    }

    return res;
}

std::vector<STMT_LO> StatementTable::getStmtOfType(StatementType type) {
    std::vector<STMT_LO> res;

    for (const auto& row : rows) {
        if (row.getStmt().type.value() == type) {
            res.push_back(row.getStmt());
        }
    }

    return res;
}

StatementType StatementTable::getStmtTypeOfLine(int statementNum) {
    std::vector<StatementRow> filtered;
    std::copy_if(begin(rows), end(rows), std::back_inserter(filtered), 
        [statementNum](StatementRow row) { return row.getStmt().statementNum == statementNum; });
    
    if (filtered.size() == 0) {
        throw "No statement exists with the provided statement number"; 
    } else if (filtered.size() > 1) {
        throw "Statement table has rows with duplicate line numbers";
    } else {
        STMT_LO stmt = filtered.at(0).getStmt();

        if (stmt.type.has_value()) {
            return stmt.type.value();
        } else {
            throw "Statement does not have a type";
        }
    }
}
