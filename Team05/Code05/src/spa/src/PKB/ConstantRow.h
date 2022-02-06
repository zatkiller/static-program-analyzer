#pragma once

#include "PKBDataTypes.h"

class ConstantRow {
public:
    explicit ConstantRow(CONST value) : constant(value) {}

    bool operator == (const ConstantRow&) const;
    CONST getConst() const;
    
private:
    CONST constant;
};

class ConstantRowHash {
public:
    size_t operator() (const ConstantRow&) const;
};
