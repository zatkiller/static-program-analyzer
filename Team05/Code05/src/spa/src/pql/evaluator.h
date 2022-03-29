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
#include "pql/clausehandler.h"
#include "PKB/PKBField.h"
#include "PKB/PKBResponse.h"
#include "PKB.h"
#include "PKB/PKBCommons.h"

namespace qps::evaluator {
struct SelectElemInfo {
    int columnNo;
    query::AttrName attrName;
    bool isAttr;

    static SelectElemInfo ofDeclaration(int columnNo);
    static SelectElemInfo ofAttr(int columnNo, query::AttrName attrName);
};
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
     * Wraps a certain PKBFiend into a string representation.
     *
     * @param pkbField the PKBField to wrap
     * @return string representation of the PKBField
     */
    static std::string PKBFieldToString(PKBField pkbField);

    std::string PKBFieldAttrToString(PKBField pkbField, query::AttrName attrName);
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
     * Retrieves the final result from the result table.
     *
     * @param table the reference the result table
     * @param variable the synonym in select part of the query
     * @return the list of string representation of the query result
     */
    std::list<std::string> getListOfResult(ResultTable &table, query::ResultCl resultCl);

    /**
     * Evaluates the query object.
     *
     * @param query the query object to evaluate
     * @return the list of string representation of the query result
     */
    std::list<std::string> evaluate(query::Query query);
};
}  // namespace qps::evaluator
