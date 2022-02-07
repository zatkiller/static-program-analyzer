#pragma once

#include <variant>
#include <vector>

#include "PKBType.h"
#include "PKBDataTypes.h"

using Content = std::variant<STMT_LO, VAR_NAME, PROC_NAME, CONST>;

/**
* A data structure to represent a program design entity.
*/
struct PKBField {
    /* Type of program design entity */
    PKBType tag;

    /* Represents whether this PKBField denotes a wildcard, _. */
    bool isConcrete;
    Content content;

    // TODO: add another constructor for wildcards that takes in just the tag
    PKBField(PKBType type, bool concrete, Content c) : tag(type), isConcrete(concrete), content(c) {}
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
