#include <list>

#include "pql/qps.h"
#include "pql/query.h"
#include "pql/evaluator.h"

namespace qps {
    void QPS::evaluate(std::string query_str, std::list<std::string> &results, PKB *pkbPtr) {
        qps::query::Query query = parser.parsePql(query_str);
        if (!query.isValid()) {
            results.push_back("Invalid query!");
            return;
        }
        qps::evaluator::Evaluator evaluator(pkbPtr);
        results = evaluator.evaluate(query);
    }
}
