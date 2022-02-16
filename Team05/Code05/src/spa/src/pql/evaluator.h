#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <set>
#include <algorithm>
#include <iterator>
#include <memory>
#include <variant>
#include <map>
#include <list>

#include "pql/query.h"
#include "PKB/PKBField.h"
#include "PKB/PKBDataTypes.h"
#include "PKB/PKBEntityType.h"
#include "PKB/StatementType.h"
#include "PKB/PKBResponse.h"
#include "PKB.h"
#include "PKB/PKBRelationship.h"

class Evaluator {
    PKB* pkb;

public:
    explicit Evaluator(PKB* pkb) {
        this->pkb = pkb;
    }

    PKBResponse getAll(DesignEntity);

    std::string PKBFieldToString(PKBField);

    std::list<std::string > getListOfResult(PKBResponse);

    std::list<std::string > evaluate(Query);
};