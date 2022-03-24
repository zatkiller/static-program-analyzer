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
    return stmts.count(stmtNum) != 0;
}

bool StatementTable::contains(StatementType stmtType, int stmtNum) const {
    return this->containsVal(STMT_LO(stmtNum, stmtType));
}

void StatementTable::insert(STMT_LO stmt) {
    int statementNum = stmt.statementNum;
    if (stmts.count(statementNum) == 0) {
        insertVal(stmt);
        stmts.emplace(statementNum, stmt);
    }
}

std::optional<STMT_LO> StatementTable::getStmt(int statementNumber) const {
    auto search = stmts.find(statementNumber);
    if (search != stmts.end()) {
        return search->second;
    }

    return std::nullopt;
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
    auto stmt = getStmt(stmtNum);

    if (stmt.has_value()) {
        return stmt.value().type;
    } else {
        Logger(Level::INFO) << "No statement exists with the provided statement number";
        return std::nullopt;
    }
}
