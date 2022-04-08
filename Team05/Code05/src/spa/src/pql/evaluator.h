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
#include <utility>

#include "pql/query.h"
#include "pql/resultprojector.h"
#include "PKB/PKBField.h"
#include "PKB/PKBResponse.h"
#include "PKB.h"
#include "PKB/PKBCommons.h"

namespace qps::evaluator {
/**
 * A class to evaluate a query object. Get the corresponding result from PKB and merge results accordingly.
 */
class Evaluator {
    PKB *pkb;
    std::vector<ResultTable> intermediateTables;
    ResultTable resultTable;

public:
    /** Constructor for the evaluator. */
    explicit Evaluator(PKB *pkb) {
        this->pkb = pkb;
    }

    std::vector<ResultTable> findResultRelatedGroup(std::vector<std::string> selectSyns);

    ResultTable mergeGroupResults(std::vector<ResultTable> tables);

    /**
     * Evaluates the query object.
     *
     * @param query the query object to evaluate
     * @return the list of string representation of the query result
     */
    std::list<std::string> evaluate(query::Query query);
};
}  // namespace qps::evaluator
