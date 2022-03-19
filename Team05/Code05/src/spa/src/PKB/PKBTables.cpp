#include "PKBTables.h"
#include "logging.h"

/* ---------------------------------ProcedureTable Methods ----------------------------------------*/
bool ProcedureTable::contains(std::string procName) const {
    return this->containsVal(PROC_NAME(procName));
}

void ProcedureTable::insert(PROC_NAME proc) {
    insertVal(proc);
}

/* ---------------------------------ConstantTable Methods ----------------------------------------*/

bool ConstantTable::contains(CONST value) const {
    return this->containsVal(value);
}

void ConstantTable::insert(CONST value) {
    insertVal(value);
}

/* ---------------------------------VariableTable Methods ----------------------------------------*/

bool VariableTable::contains(std::string varName) const {
    return this->containsVal(VAR_NAME(varName));
}

void VariableTable::insert(VAR_NAME var) {
    insertVal(var);
}

/* ---------------------------------StatementTable Methods ----------------------------------------*/

bool StatementTable::contains(int stmtNum) const {
    return this->getStmtTypeOfLine(stmtNum).has_value();
}

bool StatementTable::contains(StatementType stmtType, int stmtNum) const {
    return this->containsVal(STMT_LO(stmtNum, stmtType));
}

void StatementTable::insert(STMT_LO stmt) {
    if (!contains(stmt.statementNum)) {
        insertVal(stmt);
    }
}

std::vector<STMT_LO> StatementTable::getStmts(int statementNumber) const {
    std::vector<STMT_LO> res;

    for (const auto& row : rows) {
        if (row.getVal().statementNum == statementNumber) {
            res.push_back(row.getVal());
        }
    }
    return res;
}

std::vector<STMT_LO> StatementTable::getStmtOfType(StatementType stmtType) const {
    std::vector<STMT_LO> res;

    for (const auto& row : rows) {
        if (row.getVal().type.value() == stmtType) {
            res.push_back(row.getVal());
        }
    }

    return res;
}

std::optional<StatementType> StatementTable::getStmtTypeOfLine(int stmtNum) const {
    std::vector<EntityRow<STMT_LO>> filtered;
    std::copy_if(begin(rows), end(rows), std::back_inserter(filtered),
        [stmtNum](EntityRow<STMT_LO> row) { return row.getVal().statementNum == stmtNum; });

    if (filtered.size() == 0) {
        Logger(Level::INFO) << "No statement exists with the provided statement number";
        return std::nullopt;
    } else if (filtered.size() > 1) {
        Logger(Level::INFO) << "Statement table has rows with duplicate line numbers";
        return std::nullopt;
    } else {
        STMT_LO stmt = filtered.at(0).getVal();

        if (stmt.type.has_value()) {
            return stmt.type.value();
        } else {
            Logger(Level::INFO) << "Statement does not have a type";
            return std::nullopt;
        }
    }
}
