#include "PKBResponse.h"

/** =================================== PKBRESPONSE METHODS =================================== */

bool PKBResponse::operator == (const PKBResponse& other) const {
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
            std::unordered_set<PKBField, PKBFieldHash> cont1 = std::get<0>(res);
            std::unordered_set<PKBField, PKBFieldHash> cont2 = std::get<0>(other.res);
            return cont1 == cont2;
        }
        case 1:
        {
            std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> cont1 = std::get<1>(res);
            std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> cont2 = std::get<1>(other.res);
            return cont1 == cont2;
        }
        default:
            return false;
        }
    }
}
