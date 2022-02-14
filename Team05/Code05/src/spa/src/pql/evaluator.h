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
#include "PKB/PKBEntityType.h"
#include "PKB/StatementType.h"
#include "PKB/PKBResponse.h"
#include "PKB.h"
#include "PKB/PKBRelationship.h"

namespace qps::evaluator {
    using namespace qps::query;
    using namespace qps::evaluator;

    class Evaluator {
        PKB *pkb;
        ResultTable resultTable;

    public:
        Evaluator(PKB *pkb) {
            this->pkb = pkb;
            this->resultTable = ResultTable();
        }

        PKBResponse getAll(DesignEntity);

        static std::string PKBFieldToString(PKBField);

        void processSuchthat(std::vector<std::shared_ptr<RelRef>>, std::vector<std::shared_ptr<RelRef>> &,
                             std::vector<std::shared_ptr<RelRef>> &);

        template<typename F1, typename F2>
        void processSuchthatRelRef(std::shared_ptr<RelRef> r, F1 f1, F2 f2, std::vector<std::shared_ptr<RelRef>> &noSyn,
                                   std::vector<std::shared_ptr<RelRef>> &hasSyn) {
            if (!f1.isDeclaration() && !f2.isDeclaration()) {
                noSyn.push_back(r);
            } else {
                hasSyn.push_back(r);
            }
        }

        std::list<std::string> getListOfResult(ResultTable &, std::string);

        std::list<std::string> evaluate(Query);
    };
}
