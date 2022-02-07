#pragma once

#include <unordered_set>
#include <stdio.h>
#include <vector>
#include <variant>

#include "PKBField.h"

using FieldResponse = std::unordered_set<PKBField, PKBFieldHash>;

using FieldRowResponse = std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>;

using Response = std::variant<FieldResponse, FieldRowResponse>;

struct PKBResponse {
    bool hasResult;
    Response res;

    bool operator == (const PKBResponse& other) const {
        if (hasResult != other.hasResult) {
            return false;
        } else if (!hasResult && !other.hasResult) {
            return true;
        } else {
            int currInd = res.index();
            int otherInd = other.res.index();

            if (currInd != otherInd) {
                return false;
            }

            switch (currInd) {
            case 0:
            {
                FieldResponse cont1 = std::get<0>(res);
                FieldResponse cont2 = std::get<0>(other.res);
                return cont1 == cont2;
            }
            case 1:
            {
                FieldRowResponse cont1 = std::get<1>(res);
                FieldRowResponse cont2 = std::get<1>(other.res);
                return cont1 == cont2;
            }
            default:
                break;
            }
        }
    }
};
