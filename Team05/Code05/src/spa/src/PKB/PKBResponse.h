#pragma once

#include <unordered_set>
#include <stdio.h>
#include <vector>
#include <variant>

#include "PKBField.h"

using Response = std::variant<std::unordered_set<PKBField, PKBFieldHash>,
    std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>>;

/**
* A data structure representing the results of a query (PKB GET APIs).
*/
struct PKBResponse {
    bool hasResult; // TODO: is this required?

    /**
    * Results of a query is stored in a set. To obtain the set, include 
    * *(std::get_if<std::unordered_set<PKBField, PKBFieldHash>>(&PKBResponse.res).
    */
    Response res;
};
