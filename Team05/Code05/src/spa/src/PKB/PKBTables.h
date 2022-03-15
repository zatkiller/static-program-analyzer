#pragma once

#include <set>
#include <optional>
#include <type_traits>
#include "PKBField.h"
#include "PKBRelationshipTables.h"

/**
* A generic data structure to store entities (PROC_NAME, VAR_NAME, STMT_LO, CONST) in an EntityTable
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
* A data structure to store EntityRows of type <T>
*/
template <typename T>
class EntityTable {
public:
    /**
    * Returns all entities stored in the EntityTable
    * 
    * @returns a vector of entities 
    */
    std::vector<T> getAllEntity() const {
        std::vector<T> res;
        for (auto row : rows) {
            T val = row.getVal();
            res.push_back(val);
        }
        return res;
    }

    /**
    * Returns the size of the EntityTable
    * 
    * @returns an int representing the size of the EntityTable
    */
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

/**
* A data structure to hold EntityRow<CONST>
*/
class ConstantTable : public EntityTable<CONST> {
public:
    /**
    * Checks if the provided CONST is inside the ConstantTable
    * 
    * @param value The provided CONST to check
    * @return true if the value is in the ConstantTable and false otherwise
    */
    bool contains(CONST value) const;

    /**
    * Inserts the provided CONST into the ConstantTable
    * 
    * @param value The provided CONST to insert into the table
    */
    void insert(CONST value);
};

/**
* A data structure to hold EntityRow<PROC_NAME>
*/
class ProcedureTable : public EntityTable<PROC_NAME> {
public:
    /**
    * Checks if the provided procedure name is in the ProcedureTable
    * 
    * @param procName The provided procedure name to check
    * @returns true if the value is in the ProcedureTable and false otherwise
    */
    bool contains(std::string procName) const;

    /**
    * Inserts the provided PROC_NAME into the ProcedureTable
    * 
    * @param procName The string value of the procedure name to be inserted
    */
    void insert(std::string procName);
};

/**
* A data structure to hold EntityRow<VAR_NAME>
*/
class VariableTable : public EntityTable<VAR_NAME> {
public:
    /**
    * Checks if the provided variable name is in the VariableTable
    *
    * @param varName The provided variable name to check
    * @returns true if the value is in the VariableTable and false otherwise
    */
    bool contains(std::string varName) const;

    /**
    * Inserts the provided variable name into the VariableTable
    *
    * @param varName The provided variable name to insert into the VariableTable
    */
    void insert(std::string varName);
};

/**
* A data structure to hold EntityRow<STMT_LO>
*/
class StatementTable : public EntityTable<STMT_LO> {
public:
    /**
    * Checks if the provided statement number is in the StatementTable
    *
    * @param stmtNum The provided statement number to check
    * @return true if the provided statement is in the StatementTable and false otherwise
    */
    bool contains(int stmtNum) const;

    /**
    * Checks if a statement matching the provided statement type and number is in
    * the StatementTable
    *
    * @param stmtType The provided statement type to check
    * @param stmtNum The provided statement number to check
    * @return true if the provided statement is in the StatementTable and false otherwise
    */
    bool contains(StatementType stmtType, int stmtNum) const;

    /**
    * Inserts a (assignment, if, or while) statement with the provided statement type and statement num
    * into the StatementTable
    *
    * @param stmtType The provided statement type to insert
    * @param stmtNum The provided statement number to insert
    */
    void insert(int stmtNum, StatementType stmtType);

    /**
    * Inserts a (call, read, or print) statement with the provided statement type and statement num and attribute
    * into the StatementTable
    *
    * @param stmtType The provided statement type to insert
    * @param stmtNum The provided statement number to insert
    * @param attribute The provided attribute (VAR_NAME or PROC_NAME) to insert
    */
    void insert(int stmtNum, StatementType stmtType, std::string attribute);

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
    * @param statementNum The provided statement number to look up
    * @return std::optional<StatementType> The statement type of the provided statement
    *   if it exists
    */
    std::optional<StatementType> getStmtTypeOfLine(int statementNum) const;

    /**
    * Retrieves a vector of STMT_LOs with the statement number provided. 
    * 
    * This method should only return an empty vector or a vector with 1 element.
    * 
    * @param statementNumber the provided statement number to look up
    * @return std::vector<STMT_LO> a vector of STMT_LOs that matches the provided statement number
    */
    std::vector<STMT_LO> getStmts(int statementNumber) const;
};
