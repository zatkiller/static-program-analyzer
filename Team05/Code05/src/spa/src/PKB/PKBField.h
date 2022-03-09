#pragma once

#include <variant>
#include <vector>
#include <stdexcept>
#include <optional>
#include <string>

#include "PKBCommons.h"

enum class PKBEntityType {
    STATEMENT, VARIABLE, PROCEDURE, CONST
};

enum class PKBFieldType {
    CONCRETE, DECLARATION, WILDCARD
};

typedef int CONST;

typedef struct VAR_NAME {
    VAR_NAME(std::string str) : name(str) {}
    std::string name;

    bool operator == (const VAR_NAME&) const;
    bool operator < (const VAR_NAME&) const;
} VAR_NAME;

typedef struct PROC_NAME {
    PROC_NAME(std::string str) : name(str) {}
    std::string name;

    bool operator == (const PROC_NAME&) const;
    bool operator < (const PROC_NAME&) const;
} PROC_NAME;

using StatementAttribute = std::variant<VAR_NAME, PROC_NAME>;

typedef struct STMT_LO {
    STMT_LO(int statementNum) : statementNum(statementNum), type(std::nullopt), attribute(std::nullopt) {}
    STMT_LO(int statementNum, StatementType type) : statementNum(statementNum), type(type), attribute(std::nullopt) {}
    STMT_LO(int statementNum, StatementType type, StatementAttribute attribute) : 
        statementNum(statementNum), type(type), attribute(attribute) {}

    int statementNum;
    std::optional<StatementType> type;
    std::optional<StatementAttribute> attribute;

    bool hasStatementType();
    bool hasAttribute();
    bool operator == (const STMT_LO&) const;
    bool operator < (const STMT_LO&) const;
} STMT_LO;


using Content = std::variant<std::monostate, STMT_LO, VAR_NAME, PROC_NAME, CONST>;

/**
* A data structure to represent a program design entity.
*/
struct PKBField {
    PKBFieldType fieldType; /**< Type of field */
    PKBEntityType entityType; /**< Type of program design entity */
    std::optional<StatementType> statementType;
    Content content;

    PKBField() {}

    /**
    * Creates a concrete field. The type of data must be a PKBEntityType.
    */
    static PKBField createConcrete(Content content) {
        PKBEntityType type{};

        switch (content.index()) {
        case 1:
            type = PKBEntityType::STATEMENT;
            break;
        case 2:
            type = PKBEntityType::VARIABLE;
            break;
        case 3:
            type = PKBEntityType::PROCEDURE;
            break;
        case 4:
            type = PKBEntityType::CONST;
            break;
        default:
            throw std::invalid_argument("Invalid entity type provided.");
        }

        return PKBField{ PKBFieldType::CONCRETE, type, content };
    }

    /**
    * Creates a wildcard representing the given entityType.
    */
    static PKBField createWildcard(PKBEntityType entityType);

    /**
     * Creates a declaration (synonym) for procedures, variables, and constants.
     */
    static PKBField createDeclaration(PKBEntityType entityType);

    /**
    * Creates a declaration (synonym) for statements.
    */
    static PKBField createDeclaration(StatementType statementType);

    /**
    * Get a pointer to the active data type in the Content variant. If the active type in the variant is not
    * equal to T, a nullptr is returned.
    */
    template <typename T>
    T* getContent() {
        if (fieldType == PKBFieldType::CONCRETE) {
            return std::get_if<T>(&content);
        }

        return nullptr;
    }

    /**
    * Checks whether a PKBField is valid given an entity type.
    */
    bool isValidConcrete(PKBEntityType type);

    bool operator == (const PKBField&) const;

private:
    /* Constructor for a concrete field. */
    PKBField(PKBFieldType fieldType, PKBEntityType entityType, Content content) :
        fieldType(fieldType), entityType(entityType), content(content) {
    }

    /* Constructor for a variable, or procedure, or constant field, or a wildcard field. */
    PKBField(PKBFieldType fieldType, PKBEntityType entityType) :
        fieldType(fieldType), entityType(entityType) {
    }

    /* Constructor for a statement declaration field. */
    PKBField(PKBFieldType fieldType, PKBEntityType entityType, StatementType statementType) :
        fieldType(fieldType), entityType(entityType), statementType(statementType) {
    }
};

/**
* Hash function for PKBField.
*/
class PKBFieldHash {
public:
    size_t operator() (const PKBField&) const;
};

/**
* Hash function for a vector of PKBFields.
*/
class PKBFieldVectorHash {
public:
    size_t operator() (const std::vector<PKBField>&) const;
};
