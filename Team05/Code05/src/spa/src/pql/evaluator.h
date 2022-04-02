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
    ResultTable resultTable;

public:
    /** Constructor for the evaluator. */
    explicit Evaluator(PKB *pkb) {
        this->pkb = pkb;
        this->resultTable = ResultTable();
    }

    /**
     * Classifies the clauses in such that into clauses with synonyms and clauses without synonyms.
     *
     * @param clauses all clauses in such that
     * @param noSyn group of clauses without synonyms
     * @param hasSyn group of clauses with synonyms
     */
    void processSuchthat(std::vector<std::shared_ptr<query::RelRef>> clauses,
                         std::vector<std::shared_ptr<query::RelRef>> &noSyn,
                         std::vector<std::shared_ptr<query::RelRef>> &hasSyn);

    /**
     *
     * @param withClauses
     * @param noAttrRef
     * @param hasAttrRef
     */
    void processWith(std::vector<query::AttrCompare> withClauses,
                     std::vector<query::AttrCompare> &noAttrRef, std::vector<query::AttrCompare> &hasAttrRef);


    /**
     * Evaluates the query object.
     *
     * @param query the query object to evaluate
     * @return the list of string representation of the query result
     */
    std::list<std::string> evaluate(query::Query query);
};
}  // namespace qps::evaluator
