#pragma once

#include <unordered_set>
#include <stdio.h>
#include <vector>
#include <variant>

#include "PKBField.h"

using FieldResponse = std::unordered_set<PKBField, PKBFieldHash>;

using FieldRowResponse = std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>;

using Response = std::variant<FieldResponse, FieldRowResponse>;

/**
* A data structure representing the results of a query (PKB GET APIs).
*/
struct PKBResponse {
    // TODO(@teo-jun-xiong): is this required?
    bool hasResult;

    /**
      * Results of a query is stored in a set. To obtain the set, include
      * *(std::get_if<std::unordered_set<PKBField, PKBFieldHash>>(&PKBResponse.res).
      */
    Response res;

    /**
    * Get a pointer to the active data type in the Response variant. If the active type in the variant is not
    * equal to T, a nullptr is returned.
    */
    template <typename T>
    T* getResponse() {
        return std::get_if<T>(&res);
    }

    bool operator == (const PKBResponse& other) const;
};
