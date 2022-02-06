#pragma once

#include <variant>
#include <vector>

#include "PKBType.h"
#include "PKBDataTypes.h"

using Content = std::variant<STMT_LO, VAR_NAME, PROC_NAME, CONST>;

struct PKBField {
    PKBType tag;
    bool isConcrete;
    Content content;

    public:
        PKBField(PKBType type, bool concrete, Content c) : tag(type), isConcrete(concrete), content(c) {}
        PKBField() {}

        bool operator == (const PKBField&) const;
};

class PKBFieldHash {
public:
    size_t operator() (const PKBField&) const;
};

class PKBFieldVectorHash {
public:
    size_t operator() (const std::vector<PKBField>&) const;
};
