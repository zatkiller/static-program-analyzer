#pragma once

#include <set>
#include <optional>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <memory>

#include "PKBField.h"

/**
* An interface for a data structure used in an EntityTable.
*/
template <typename T>
class EntityDataStructure {
public:
    /**
    * Checks whether the data structure contains the given value.
    * 
    * @param val program design entity
    * @return bool
    */
    virtual bool contains(T val) const = 0;

    /**
    * Inserts the given value into the data structure.
    * 
    * @param val program design entity
    */
    virtual void insert(T val) = 0;

    /**
    * Returns the number of entries in the data structure.
    *
    * @return int
    */
    virtual int getSize() const = 0;

    /**
    * Retrieves all the entries stored.
    *
    * @return a vector of entities
    */
    virtual std::vector<T> getAllEntities() const = 0;
};

/**
* A generic data structure to store entities (PROC_NAME, VAR_NAME, CONST) in an EntitySet in an EntityTable
*/
template <typename T>
class EntityRow {
public:
    explicit EntityRow<T>(T value) : val(value) {}

    /**
    * Retrieves the value stored in the EntityRow<T>
    *
    * @return a value of type T
    */
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

/**
* Hash function for a generic EntityRow
*/
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

/**
* A generic set to store entities (PROC_NAME, VAR_NAME, CONST) in an EntityTable. Inherits EntityDataStructure.
*/
template <typename T>
class EntitySet : public EntityDataStructure<T> {
public:
    /**
    * Checks whether the set contains the given program design entity.
    *
    * @param val program design entity
    * @return bool
    */
    bool contains(T val) const override {
        return rows.count(EntityRow<T>(val)) != 0;
    }

    /**
    * Insert the given program design entity into the set.
    *
    * @param val program design entity
    */
    void insert(T val) override {
        rows.insert(EntityRow<T>(val));
    }

    /**
    * Returns the number of entries in the data structure.
    *
    * @return int
    */
    int getSize() const override {
        return rows.size();
    }

    /**
    * Retrieves all the entries stored.
    *
    * @return a vector of entities
    */
    std::vector<T> getAllEntities() const override {
        std::vector<T> res;
        for (auto row : rows) {
            T val = row.getVal();
            res.push_back(val);
        }
        return res;
    }

protected:
    std::unordered_set<EntityRow<T>, EntityRowHash<T>> rows;
};

/**
* A map to store statements (STMT_LO) in an EntityTable. Inherits EntityDataStructure.
*/
class StatementMap : public EntityDataStructure<STMT_LO> {
public:
    /**
    * Checks whether the map contains the given statement.
    *
    * @param val a statement
    * @return bool
    */
    bool contains(STMT_LO val) const override;

    /**
    * Checks whether the map contains a statement with the given statement number.
    *
    * @param statementNumber a statement number
    * @return bool
    */
    bool contains(int statementNumber) const;

    /**
    * Checks whether the map contains a statement with the given statement number and the given statement type.
    *
    * @param statementNumber a statement number
    * @param type a statement type
    * @return bool
    */
    bool contains(StatementType type, int statementNumber) const;
    
    /**
    * Inserts the given statement into the map.
    * 
    * @param val a statement
    */
    void insert(STMT_LO val) override;

    /**
    * Returns the number of entries in the data structure.
    *
    * @return int
    */
    int getSize() const override;
    
    /**
    * Retrieves all the statements stored.
    *
    * @return a vector of STMT_LOs
    */
    std::vector<STMT_LO> getAllEntities() const override;

    /**
    * Retrieves the statement with the given statement number.
    *
    * @return an optional STMT_LO
    */
    std::optional<STMT_LO> getStmt(int statementNumber) const;

    /**
    * Retrieves all the statements with the given statement type
    *
    * @return a vector of STMT_LOs
    */
    std::vector<STMT_LO> getStmtsOfType(StatementType type) const;

private:
    // unordered_map to get O(1) average, assumes that query will always have result
    std::unordered_map<int, STMT_LO> entities;
};

/**
* A data structure to store program design entities in a EntityDataStructure
*/
template <typename T>
class EntityTable {
public:
    explicit EntityTable(std::variant<StatementMap, EntitySet<T>> entities): entities(entities) {}

    /**
    * Returns all entities stored in the EntityTable
    *
    * @return a vector of entities
    */
    std::vector<T> getAllEntity() const {
        if constexpr (std::is_same_v<T, STMT_LO>) {
            return std::get<StatementMap>(entities).getAllEntities();
        } else {
            return std::get<EntitySet<T>>(entities).getAllEntities();
        }
    }

    /**
    * Returns the size of the EntityTable
    *
    * @return int
    */
    int getSize() const {
        if constexpr (std::is_same_v<T, STMT_LO>) {
            return std::get<StatementMap>(entities).getSize();
        } else {
            return std::get<EntitySet<T>>(entities).getSize();
        }
    }

    /**
    * Checks if the EntityDataStructure in the EntityTable contains the given program design entity.
    * 
    * @param val program design entity
    * @return bool
    */
    bool contains(T val) const {
        if constexpr (std::is_same_v<T, STMT_LO>) {
            return std::get<StatementMap>(entities).contains(val);
        } else {
            return std::get<EntitySet<T>>(entities).contains(val);
        }
    }

    /**
    * Inserts the given program design entity into the EntityDataStructre in the EntityTable.
    */
    void insert(T val) {
        if constexpr (std::is_same_v<T, STMT_LO>) {
            std::get<StatementMap>(entities).insert(val);
        } else {
            std::get<EntitySet<T>>(entities).insert(val);
        }
    }

protected:
    std::variant<StatementMap, EntitySet<T>> entities;
};

/**
* A data structure to hold EntityRow<CONST>
*/
class ConstantTable : public EntityTable<CONST> {
public:
    ConstantTable();
};

/**
* A data structure to hold EntityRow<PROC_NAME>
*/
class ProcedureTable : public EntityTable<PROC_NAME> {
public:
    ProcedureTable();
};

/**
* A data structure to hold EntityRow<VAR_NAME>
*/
class VariableTable : public EntityTable<VAR_NAME> {
public:
    VariableTable();
};

/**
* A data structure to hold EntityRow<STMT_LO>
*/
class StatementTable : public EntityTable<STMT_LO> {
public:
    StatementTable();

    /**
    * Checks if the provided statement number is in the StatementTable
    *
    * @param statementNumber The provided statement number to check
    * @return true if the provided statement is in the StatementTable and false otherwise
    */
    bool contains(int statementNumber) const;

    /**
    * Checks if a statement matching the provided statement type and number is in
    * the StatementTable
    *
    * @param stmtType The provided statement type to check
    * @param statementNumber The provided statement number to check
    * @return true if the provided statement is in the StatementTable and false otherwise
    */
    bool contains(StatementType stmtType, int statementNumber) const;

    /**
    * Retrieves all statements of the provided statement type
    *
    * @param type The provided statement type to filter for
    * @return std::vector<STMT_LO> A vector of STMT_LOs that match the
    *    provided statement type
    */
    std::vector<STMT_LO> getStmtOfType(StatementType type) const;

    /**
    * Gets the statement type of the statement at the provided statement number
    *
    * @param statementNumber The provided statement number to look up
    * @return std::optional<StatementType> The statement type of the provided statement
    *   if it exists
    */
    std::optional<StatementType> getStmtTypeOfLine(int statementNumber) const;

    /**
    * Retrieves a vector of STMT_LOs with the statement number provided.
    *
    * This method should only return an empty vector or a vector with 1 element.
    *
    * @param statementNumber the provided statement number to look up
    * @return std::vector<STMT_LO> a vector of STMT_LOs that matches the provided statement number
    */
    std::optional<STMT_LO> getStmt(int statementNumber) const;
};
