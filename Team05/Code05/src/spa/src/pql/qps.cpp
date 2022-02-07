#include <list>

#include "pql/qps.h"
#include "pql/query.h"
#include "pql/evaluator.h"


void QPS::evaluate(std::string query_str, std::list<std::string> &results, PKB* pkbPtr) {
    Query query = parser.parsePql(query_str);
    Evaluator evaluator(pkbPtr);
    results = evaluator.evaluate(query);
}
