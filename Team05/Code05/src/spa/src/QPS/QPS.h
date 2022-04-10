#pragma once

#include <string>
#include <list>

#include "QPS/Parser.h"
#include "QPS/Evaluator.h"

namespace qps {

/**
 * Struct used to represent the QPS component
 */
struct QPS {
    qps::parser::Parser parser;

    /**
     * Evaluates a query and stores the query results in a list of string
     *
     * @param query the QPS query
     * @param results the list to store the QPS query results in
     * @param pkbPtr the pointer to the pkb
     */
    void evaluate(const std::string& query, std::list<std::string> &results, PKB *pkbPtr);
};

}  // namespace qps
