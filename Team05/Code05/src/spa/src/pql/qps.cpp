#include <list>
#include <string_view>

#include "exceptions.h"
#include "pql/qps.h"
#include "pql/query.h"
#include "pql/evaluator.h"

namespace qps {
    void QPS::evaluate(const std::string& query_str, std::list<std::string> &results, PKB *pkbPtr) {
        qps::query::Query query = parser.parsePql(std::string_view(query_str));
        if (!query.isValid())
            return;

        qps::evaluator::Evaluator evaluator(pkbPtr);
        try {
            results = evaluator.evaluate(query);
        } catch (exceptions::PqlException) {
            return;
        }
    }
}  // namespace qps
