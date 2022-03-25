#include "PKBTables.h"
#include "logging.h"

/* --------------------------------- StatementMap Methods ----------------------------------------*/
bool StatementMap::contains(STMT_LO val) const {
    auto search = entities.find(val.statementNum);
    if (search != entities.end() && search->second == val) {
        return true;
    } 

    return false;
}

void StatementMap::insert(STMT_LO val) {
    entities.emplace(val.statementNum, val);
}

int StatementMap::getSize() const {
    return entities.size();
}

std::vector<STMT_LO> StatementMap::getAllEntities() const {
    std::vector<STMT_LO> res;
    for (auto it = entities.begin(); it != entities.end(); ++it) {
        res.push_back(it->second);
    }
    return res;
}

bool StatementMap::contains(int statementNumber) const {
    return entities.count(statementNumber) != 0;
}

bool StatementMap::contains(StatementType type, int statementNumber) const {
    auto stmt = getStmt(statementNumber);
    if (stmt.has_value()) {
        return stmt.value().type.value() == type;
    }
    
    return false;
}

std::optional<STMT_LO> StatementMap::getStmt(int statementNumber) const {
    auto search = entities.find(statementNumber);
    if (search != entities.end()) {
        return search->second;
    }

    return std::nullopt;
}

std::vector<STMT_LO> StatementMap::getStmtsOfType(StatementType type) const {
    std::vector<STMT_LO> res;

    for (auto it = entities.begin(); it != entities.end(); ++it) {
        if (it->second.type.value() == type) {
            res.push_back(it->second);
        }
    }

    return res;
}

/* --------------------------------- ProcedureTable Methods ----------------------------------------*/
ProcedureTable::ProcedureTable() : EntityTable(EntitySet<PROC_NAME>{}) {}

/* --------------------------------- ConstantTable Methods ----------------------------------------*/
ConstantTable::ConstantTable() : EntityTable(EntitySet<CONST>{}) {}

/* --------------------------------- VariableTable Methods ----------------------------------------*/
VariableTable::VariableTable() : EntityTable(EntitySet<VAR_NAME>{}) {}

/* --------------------------------- StatementTable Methods ----------------------------------------*/
StatementTable::StatementTable() : EntityTable(StatementMap{}) {}

bool StatementTable::contains(int statementNumber) const {
    auto statementMap = std::get<StatementMap>(entities);
    return statementMap.contains(statementNumber);
}

bool StatementTable::contains(StatementType type, int statementNumber) const {
    auto statementMap = std::get<StatementMap>(entities);
    auto stmt = statementMap.getStmt(statementNumber);
    if (stmt.has_value()) {
        return stmt.value().type.value() == type;
    }

    return false;
}

std::vector<STMT_LO> StatementTable::getStmtOfType(StatementType type) const {
    auto statementMap = std::get<StatementMap>(entities);
    return statementMap.getStmtsOfType(type);
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
    auto statementMap = std::get<StatementMap>(entities);
    return statementMap.getStmt(statementNumber);
}
