#pragma once

#include <variant>
#include <vector>

#include "PKBEntityType.h"
#include "PKBDataTypes.h"
#include "PKBFieldType.h"
#include <optional>

using Content = std::variant<std::monostate, STMT_LO, VAR_NAME, PROC_NAME, CONST>;

/**
* A data structure to represent a program design entity.
*/
struct PKBField {
    /* Type of field */
    PKBFieldType fieldType;

    /* Type of program design entity */
    PKBEntityType entityType;

    std::optional<StatementType> statementType;
    Content content;

    /* Constructor for a concrete field. */
    PKBField(PKBFieldType fieldType, PKBEntityType entityType, Content content) :
        fieldType(fieldType), entityType(entityType), content(content) {}

    /* Constructor for a variable, or procedure, or constant field, or a wildcard field. */
    PKBField(PKBFieldType fieldType, PKBEntityType entityType) :
        fieldType(fieldType), entityType(entityType) {}

    /* Constructor for a statement declaration field. */
    PKBField(PKBFieldType fieldType, PKBEntityType entityType, StatementType statementType) :
        fieldType(fieldType), entityType(entityType), statementType(statementType) {}

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

    bool operator == (const PKBField&) const;
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
