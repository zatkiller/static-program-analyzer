#pragma once

#include <set>
#include <optional>
#include <type_traits>
#include "PKBField.h"
#include "PKBRelationshipTables.h"

template <typename T>
class EntityRow {
public:
    explicit EntityRow<T>(T value) : val(value) {}

    T getVal() const {
        return val;
    }

    template <typename U>
    bool operator == (const EntityRow<U>& other) const {
        if constexpr (std::is_same<T, U>::value) {
            return this->val == other.getVal();
        }
        return false;
    }

private: 
    T val;
};

template <typename T>
class EntityRowHash {
public:
    size_t operator() (const EntityRow<T>& other) const {
        if constexpr (std::is_same_v<T, STMT_LO>) {
            return std::hash<int>()(other.getVal().statementNum);
        } else if constexpr (std::is_same_v<T, PROC_NAME>) {
            return std::hash<std::string>()(other.getVal().name);
        } else if constexpr (std::is_same_v<T, CONST>) {
            return std::hash<int>()(other.getVal());
        } else {
            return std::hash<std::string>()(other.getVal().name);
        }
    }
};

template <typename T>
class EntityTable {
public:
    std::vector<T> getAllEntity() const {
        std::vector<T> res;
        for (auto row : rows) {
            T val = row.getVal();
            res.push_back(val);
        }
        return res;
    }

    int getSize() const {
        return rows.size();
    }

protected:
    bool containsVal(T val) const {
        return rows.count(EntityRow<T>(val)) == 1;
    }

    void insertVal(T val) {
        rows.insert(EntityRow<T>(val));
    }

    std::unordered_set<EntityRow<T>, EntityRowHash<T>> rows;
};

class ConstantTable : public EntityTable<CONST> {
public:
    bool contains(CONST value) const;
    void insert(CONST value);
};

class ProcedureTable : public EntityTable<PROC_NAME> {
public:
    bool contains(std::string procName) const;
    void insert(std::string procName);
};

class VariableTable : public EntityTable<VAR_NAME> {
public:
    bool contains(std::string varName) const;
    void insert(std::string varName);
};

class StatementTable : public EntityTable<STMT_LO> {
public:
    bool contains(int stmtNum) const;
    bool contains(StatementType stmtType, int stmtNum) const;
    void insert(StatementType stmtType, int stmtNum);
    std::vector<STMT_LO> getStmtOfType(StatementType type) const;
    std::optional<StatementType> getStmtTypeOfLine(int statementNum) const;
};
