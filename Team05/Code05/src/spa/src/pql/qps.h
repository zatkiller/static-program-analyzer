#pragma once

#include <string>
#include <list>

#include "pql/parser.h"
#include "pql/evaluator.h"

namespace qps {

/**
 * Struct used to the QPS component
 */
struct QPS {
    qps::parser::Parser parser;

    /*
     * Evaluates a query and stores the query results in a list of string
     *
     * @param query the pql query
     * @param results the list to store the pql query results in
     * @param pkbPtr the pointer to the pkb
     */
    void evaluate(std::string query, std::list<std::string> &results, PKB *pkbPtr);
};

} // namespace qps
