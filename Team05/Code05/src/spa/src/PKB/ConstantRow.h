#pragma once

#include "PKBDataTypes.h"

/**
* A data structure to store constants (integers) in a ConstantTable.
*/
class ConstantRow {
public:
    explicit ConstantRow(CONST value) : constant(value) {}

    /**
    * Retrieves the constant stored in the ConstantRow.
    *
    * @return a constant wrapped in CONST
    */
    CONST getConst() const;
    bool operator == (const ConstantRow&) const;
    bool operator < (const ConstantRow& other) const;

private:
    CONST constant;
};

/**
* Hash function for ConstantRow.
*/
class ConstantRowHash {
public:
    size_t operator() (const ConstantRow&) const;
};
