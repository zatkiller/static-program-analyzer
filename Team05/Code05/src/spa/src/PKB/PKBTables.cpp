#include "PKBTables.h"
#include "logging.h"
#include <algorithm>

/** ================================ STATEMENTVECTOR METHODS ================================ */

STMT_LO NULL_STMT_LO = STMT_LO{ 0, StatementType::None };


bool isStatementNull(STMT_LO stmt) {
    return stmt == NULL_STMT_LO;
}

bool StatementVector::contains(STMT_LO val) const {
    if (val.statementNum >= entities.size()) {
        return false;
    }

    auto stmt = entities.at(val.statementNum);
    return stmt == val;
}

void StatementVector::insert(STMT_LO val) {
    while (val.statementNum >= entities.size()) {
        // fills empty elements with NULL_STMT_LO
        if (entities.size() == 0) {
            entities.resize(2, NULL_STMT_LO);
        } else {
            entities.resize(entities.size() * 2, NULL_STMT_LO);
        }
    }

    // existing entry is not replaced
    if (isStatementNull(entities.at(val.statementNum))) {
        entities.at(val.statementNum) = val;
    }
}

int StatementVector::getSize() const {
    return getAllEntities().size();
}

std::vector<STMT_LO> StatementVector::getAllEntities() const {
    std::vector<STMT_LO> res;

    std::copy_if(entities.begin(), entities.end(), std::inserter(res, res.end()),
        [](auto const& stmt) {
            return stmt != NULL_STMT_LO;
    });

    return res;
}

bool StatementVector::contains(int statementNumber) const {
    if (statementNumber >= entities.size()) {
        return false;
    }

    return !isStatementNull(entities.at(statementNumber));
}

bool StatementVector::contains(StatementType type, int statementNumber) const {
    auto stmt = getStmt(statementNumber);
    if (stmt.has_value() && !isStatementNull(stmt.value())) {
        return stmt.value().type.value() == type;
    }
    
    return false;
}

std::optional<STMT_LO> StatementVector::getStmt(int statementNumber) const {
    if (statementNumber >= entities.size()) {
        return false;
    }

    STMT_LO stmt = entities.at(statementNumber);
    if (isStatementNull(stmt)) {
        return std::nullopt;
    }

    return stmt;
}

std::vector<STMT_LO> StatementVector::getStmtsOfType(StatementType type) const {
    std::vector<STMT_LO> res;

    std::copy_if(entities.begin(), entities.end(), std::inserter(res, res.end()),
        [type](auto const& stmt) {
            return stmt != NULL_STMT_LO && stmt.type.value() == type;
    });

    return res;
}

/** ================================ PROCEDURETABLE METHODS ================================ */

ProcedureTable::ProcedureTable() : EntityTable(EntitySet<PROC_NAME>{}) {}

/** ================================ CONSTANTTABLE METHODS ================================ */

ConstantTable::ConstantTable() : EntityTable(EntitySet<CONST>{}) {}

/** ================================ VARIABLETABLE METHODS ================================ */

VariableTable::VariableTable() : EntityTable(EntitySet<VAR_NAME>{}) {}

/** ================================ STATEMENTTABLE METHODS ================================ */

StatementTable::StatementTable() : EntityTable(StatementVector{}) {}

bool StatementTable::contains(int statementNumber) const {
    auto statementVector = std::get<StatementVector>(entities);
    return statementVector.contains(statementNumber);
}

bool StatementTable::contains(StatementType type, int statementNumber) const {
    auto statementVector = std::get<StatementVector>(entities);
    auto stmt = statementVector.getStmt(statementNumber);
    if (stmt.has_value()) {
        return stmt.value().type.value() == type;
    }

    return false;
}

std::vector<STMT_LO> StatementTable::getStmtOfType(StatementType type) const {
    auto statementVector = std::get<StatementVector>(entities);
    return statementVector.getStmtsOfType(type);
}

std::optional<StatementType> StatementTable::getStmtTypeOfLine(int statementNumber) const {
    auto stmt = getStmt(statementNumber);

    if (stmt.has_value()) {
        // no need to check if type has value because only STMT_LO with StatementType can be added to StatementTable
        return stmt.value().type.value();
    }

    return std::nullopt;
}

std::optional<STMT_LO> StatementTable::getStmt(int statementNumber) const {
    auto statementVector = std::get<StatementVector>(entities);
    return statementVector.getStmt(statementNumber);
}
