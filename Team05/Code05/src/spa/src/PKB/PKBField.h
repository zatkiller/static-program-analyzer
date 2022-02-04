#pragma once

#include <variant>

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
        bool operator < (const PKBField&) const;
};
