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
#include "pql/clausehandler.h"
#include "PKB/PKBField.h"
#include "PKB/PKBDataTypes.h"
#include "PKB/PKBEntityType.h"
#include "PKB/StatementType.h"
#include "PKB/PKBResponse.h"
#include "PKB.h"
#include "PKB/PKBRelationship.h"

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
     * Retrieves all the results of a certain design entity from PKB database.
     *
     * @param type the design entity type to retrieve
     * @return PKBResponse contains all results of the design entity type
     */
    PKBResponse getAll(query::DesignEntity type);

    /**
     * Wraps a certain PKBFiend into a string representation.
     *
     * @param pkbField the PKBField to wrap
     * @return string representation of the PKBField
     */
    static std::string PKBFieldToString(PKBField pkbField);

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
     * Retrieves the final result from the result table.
     *
     * @param table the reference the result table
     * @param variable the synonym in select part of the query
     * @return the list of string representation of the query result
     */
    std::list<std::string> getListOfResult(ResultTable &table, std::string variable);

    /**
     * Evaluates the query object.
     *
     * @param query the query object to evaluate
     * @return the list of string representation of the query result
     */
    std::list<std::string> evaluate(query::Query query);
};
}  // namespace qps::evaluator
