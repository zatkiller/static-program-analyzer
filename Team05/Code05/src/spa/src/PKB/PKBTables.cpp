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

RelationshipRow::RelationshipRow(PKBField e1, PKBField e2) : field1(e1), field2(e2) {}

bool RelationshipRow::operator == (const RelationshipRow& row) const {
    return field1 == row.field1 && field2 == row.field2;
}

PKBField RelationshipRow::getFirst() const {
    return field1;
}

PKBField RelationshipRow::getSecond() const {
    return field2;
}

size_t RelationshipRowHash::operator() (const RelationshipRow& other) const {
    PKBField ent1 = other.getFirst();
    PKBField ent2 = other.getSecond();

    PKBEntityType entityType1 = ent1.entityType;
    PKBEntityType entityType2 = ent2.entityType;

    return (std::hash<PKBEntityType>()(entityType1) ^ std::hash<PKBEntityType>()(entityType2));
}

RelationshipTable::RelationshipTable(PKBRelationship rsType) : type(rsType) {}

bool RelationshipTable::isRetrieveValid(PKBField field1, PKBField field2) {
    // both Modifies and Uses cannot accept a wildcard as its first parameter
    return field1.fieldType != PKBFieldType::WILDCARD &&
        (field1.entityType == PKBEntityType::PROCEDURE ||
            field1.entityType == PKBEntityType::STATEMENT) &&
        (field2.entityType == PKBEntityType::VARIABLE);
}

bool RelationshipTable::isInsertOrContainsValid(PKBField field1, PKBField field2) {
    return (field1.isValidConcrete(PKBEntityType::STATEMENT) || field1.isValidConcrete(PKBEntityType::PROCEDURE))
        && field2.isValidConcrete(PKBEntityType::VARIABLE);
}

bool RelationshipTable::contains(PKBField field1, PKBField field2) {
    if (!isInsertOrContainsValid(field1, field2)) {
        Logger(Level::ERROR) <<
            "RelationshipTable can only contain concrete fields and STATEMENT or PROCEDURE entity types.";
        return false;
    };

    return rows.count(RelationshipRow(field1, field2)) == 1;
}

void RelationshipTable::insert(PKBField field1, PKBField field2) {
    if (!isInsertOrContainsValid(field1, field2)) {
        Logger(Level::ERROR) <<
            "RelationshipTable only allow inserts of concrete fields and STATEMENT or PROCEDURE entity types.";
        return;
    }

    rows.insert(RelationshipRow(field1, field2));
}

FieldRowResponse RelationshipTable::retrieve(PKBField field1, PKBField field2) {
    PKBFieldType fieldType1 = field1.fieldType;
    PKBFieldType fieldType2 = field2.fieldType;

    FieldRowResponse res;

    if (!isRetrieveValid(field1, field2)) {
        Logger(Level::ERROR) <<
            "Only fields of STATEMENT or PROCEDURE entity types can be retrieved from RelationshipTable.";
        return res;
    }

    if (fieldType1 == PKBFieldType::CONCRETE && fieldType2 == PKBFieldType::CONCRETE) {
        if (this->contains(field1, field2)) res.insert({ field1, field2 });

    } else if (fieldType1 == PKBFieldType::CONCRETE && fieldType2 != PKBFieldType::CONCRETE) {
        for (auto row : rows) {
            RelationshipRow curr = row;
            PKBField first = curr.getFirst();
            PKBField second = curr.getSecond();

            if (first == field1) {
                res.insert({ first,second });
            }
        }
        // either statement declaration or wildcard, or procedure wildcard/declaration
    } else if (fieldType1 != PKBFieldType::CONCRETE && fieldType2 == PKBFieldType::CONCRETE) {
        // field1 cannot be both statement and wildcard, invalid syntax
        for (auto row : rows) {
            RelationshipRow curr = row;
            PKBField first = curr.getFirst();
            PKBField second = curr.getSecond();

            // If first declaration looking for Procedures
            if (field1.entityType == PKBEntityType::PROCEDURE && first.entityType == PKBEntityType::PROCEDURE) {
                if (second == field2) {
                    res.insert({ first,second });
                }
            }

            if (field1.entityType == PKBEntityType::STATEMENT && first.entityType == PKBEntityType::STATEMENT) {
                if (second == field2) {
                    if (field1.statementType.value() == StatementType::All || field1.statementType == first.getContent<STMT_LO>()->type.value()) {
                        res.insert({ first,second });
                    }
                }
            }
        }
    } else { // both declaration/wild
        for (auto row : rows) {
            RelationshipRow curr = row;
            PKBField first = curr.getFirst();
            PKBField second = curr.getSecond();

            // If first declaration looking for Procedures
            if (field1.entityType == PKBEntityType::PROCEDURE && first.entityType == PKBEntityType::PROCEDURE) {
                res.insert({ first,second });
            }

            if (field1.entityType == PKBEntityType::STATEMENT && first.entityType == PKBEntityType::STATEMENT) {
                if (field1.statementType.value() == StatementType::All || field1.statementType == first.getContent<STMT_LO>()->type.value()) {
                    res.insert({ first,second });
                }
            }
        }
    }

    return res;
}

PKBRelationship RelationshipTable::getType() {
    return type;
}

int RelationshipTable::getSize() {
    return rows.size();
}

ModifiesRelationshipTable::ModifiesRelationshipTable() : RelationshipTable{ PKBRelationship::MODIFIES } {};

UsesRelationshipTable::UsesRelationshipTable() : RelationshipTable{ PKBRelationship::USES } {};
