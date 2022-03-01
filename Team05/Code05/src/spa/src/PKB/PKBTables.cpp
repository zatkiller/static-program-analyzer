#include "PKBTables.h"
#include "logging.h"

/** =============================== CONSTANTROW METHODS ================================ */
bool ConstantRow::operator == (const ConstantRow& other) const {
    return constant == other.constant;
}

bool ConstantRow::operator < (const ConstantRow& other) const {
    return std::tie(constant) < std::tie(other.constant);
}

CONST ConstantRow::getConst() const {
    return constant;
}

size_t ConstantRowHash::operator() (const ConstantRow& other) const {
    return std::hash<CONST>()(other.getConst());
}

/** ============================== CONSTANTTABLE METHODS =============================== */

bool ConstantTable::contains(CONST constant) const {
    return rows.count(ConstantRow(constant)) == 1;
}

void ConstantTable::insert(CONST constant) {
    rows.insert(ConstantRow(constant));
}

int ConstantTable::getSize() const {
    return rows.size();
}

std::vector<CONST> ConstantTable::getAllConst() const {
    std::vector<CONST> res;

    for (const auto& row : rows) {
        res.push_back(row.getConst());
    }

    return res;
}

/** ============================= PROCEDUREROW METHODS ================================ */

ProcedureRow::ProcedureRow(PROC_NAME p) : procedureName(p) {}

bool ProcedureRow::operator == (const ProcedureRow& row) const {
    return procedureName == row.procedureName;
}

bool ProcedureRow::operator < (const ProcedureRow& other) const {
    return std::tie(procedureName) < std::tie(other.procedureName);
}

PROC_NAME ProcedureRow::getProcName() const {
    return procedureName;
}

size_t ProcedureRowHash::operator() (const ProcedureRow& other) const {
    return std::hash<std::string>()(other.getProcName().name);
}

/** ============================ PROCEDURETABLE METHODS =============================== */

bool ProcedureTable::contains(std::string name) {
    return rows.count(ProcedureRow(PROC_NAME{ name })) == 1;
}

void ProcedureTable::insert(std::string name) {
    rows.insert(ProcedureRow(PROC_NAME{ name }));
}

int ProcedureTable::getSize() {
    return rows.size();
}

std::vector<PROC_NAME> ProcedureTable::getAllProcs() {
    std::vector<PROC_NAME> res;

    for (const auto& row : rows) {
        res.push_back(row.getProcName());
    }

    return res;
}

/** ============================== STATEMENTROW METHODS =============================== */

StatementRow::StatementRow(StatementType t, int stmtNum) : stmt{ stmtNum, t } {}

bool StatementRow::operator == (const StatementRow& row) const {
    return stmt == row.stmt;
}

bool StatementRow::operator < (const StatementRow& other) const {
    return std::tie(stmt) < std::tie(other.stmt);
}

STMT_LO StatementRow::getStmt() const {
    return stmt;
}

size_t StatementRowHash::operator() (const StatementRow& other) const {
    return std::hash<int>()(other.getStmt().statementNum);
}

/** ============================= STATEMENTTABLE METHODS ============================== */

bool StatementTable::contains(StatementType type, int statementNumber) {
    return rows.count(StatementRow(type, statementNumber)) == 1;
}

bool StatementTable::contains(int statementNumber) {
    return getStmtTypeOfLine(statementNumber).has_value();
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

std::optional<StatementType> StatementTable::getStmtTypeOfLine(int statementNum) {
    std::vector<StatementRow> filtered;
    std::copy_if(begin(rows), end(rows), std::back_inserter(filtered),
        [statementNum](StatementRow row) { return row.getStmt().statementNum == statementNum; });

    if (filtered.size() == 0) {
        Logger(Level::ERROR) << "No statement exists with the provided statement number";
        return std::nullopt;
    } else if (filtered.size() > 1) {
        Logger(Level::ERROR) << "Statement table has rows with duplicate line numbers";
        return std::nullopt;
    } else {
        STMT_LO stmt = filtered.at(0).getStmt();

        if (stmt.type.has_value()) {
            return stmt.type.value();
        } else {
            Logger(Level::ERROR) << "Statement does not have a type";
            return std::nullopt;
        }
    }
}

/** =============================== VARIABLEROW METHODS ================================ */

VariableRow::VariableRow(VAR_NAME v) : variableName(v) {}

bool VariableRow::operator == (const VariableRow& row) const {
    return variableName == row.variableName;
}

bool VariableRow::operator < (const VariableRow& other) const {
    return std::tie(variableName) < std::tie(other.variableName);
}

VAR_NAME VariableRow::getVarName() const {
    return variableName;
}

size_t VariableRowHash::operator()(const VariableRow& varRow) const {
    std::string varName = varRow.getVarName().name;
    return std::hash<std::string>()(varName);
}

/** ============================== VARIABLETABLE METHODS =============================== */

bool VariableTable::contains(std::string name) {
    return rows.count(VariableRow(VAR_NAME{ name })) == 1;
}

void VariableTable::insert(std::string name) {
    rows.insert(VariableRow(VAR_NAME{ name }));
}

int VariableTable::getSize() {
    return rows.size();
}

std::vector<VAR_NAME> VariableTable::getAllVars() {
    std::vector<VAR_NAME> res;

    for (const auto& row : rows) {
        res.push_back(row.getVarName());
    }

    return res;
}
