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
#include "handlerelations.h"
#include "PKB/PKBField.h"
#include "PKB/PKBDataTypes.h"
#include "PKB/PKBType.h"
#include "PKB/StatementType.h"
#include "PKB/PKBResponse.h"
#include "PKB.h"
#include "PKB/PKBRelationship.h"


class Evaluator {
    PKB* pkb;
    ResultTable resultTable;

public:
    Evaluator(PKB* pkb) {
        this->pkb = pkb;
        this->resultTable = ResultTable();
    }

    PKBResponse getAll(DesignEntity);

    static std::string PKBFieldToString(PKBField);

    void processSuchthat(std::vector<std::shared_ptr<RelRef>>, std::vector<std::shared_ptr<RelRef>>&, std::vector<std::shared_ptr<RelRef>>&);

    std::list<std::string > getListOfResult(ResultTable&, std::string);

    std::list<std::string > evaluate(Query);
};

