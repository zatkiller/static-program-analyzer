#pragma once

#include <variant>
#include <vector>

#include "PKBEntityType.h"

using ReturnType = std::variant<PKBEntityType, std::vector<PKBEntityType>>;

/**
* A data structure to represent the intended return type of a query, modelling 
* synonym in 'Select' synonym [ suchthat-cl ] [ pattern-cl ]. 
*/
struct PKBReturnType {
    ReturnType ret;
};
