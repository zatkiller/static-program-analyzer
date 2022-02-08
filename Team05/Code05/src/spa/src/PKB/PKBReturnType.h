#pragma once

#include <variant>
#include <vector>

#include "PKBType.h"

using ReturnType = std::variant<PKBType, std::vector<PKBType>>;

/**
* A data structure to represent the intended return type of a query, modelling 
* synonym in 'Select' synonym [ suchthat-cl ] [ pattern-cl ]. 
*/
struct PKBReturnType {
    ReturnType ret;
};
