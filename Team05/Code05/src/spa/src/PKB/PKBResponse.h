#pragma once

#include <unordered_set>
#include <stdio.h>
#include <vector>
#include <variant>

#include "PKBField.h"

using Response = std::variant<std::unordered_set<PKBField, PKBFieldHash>,
    std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>>;

struct PKBResponse {
    bool hasResult;
    Response res;

    //bool operator == (const PKBResponse)
};
