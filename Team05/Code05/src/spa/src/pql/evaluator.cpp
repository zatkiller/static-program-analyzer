#include "evaluator.h"

#define DEBUG Logger(Level::DEBUG) << "evaluator.cpp "

namespace qps::evaluator {
    std::vector<ResultTable> Evaluator::findResultRelatedGroup(std::vector<std::string> selectSyns) {
        std::vector<ResultTable> resultRelatedGroups;
        for (auto table : intermediateTables) {
            for (auto s : selectSyns) {
                if (table.synExists(s)) {
                    table.filterColumns(selectSyns);
                    resultRelatedGroups.push_back(table);
                    break;
                }
            }
        }
        return resultRelatedGroups;
    }

    ResultTable Evaluator::mergeGroupResults(std::vector<ResultTable> tables) {
        ResultTable finalResultTable = tables[0];
        for (int i = 1; i < tables.size(); i++) {
            finalResultTable.crossJoin(tables[i]);
        }
        return finalResultTable;
    }

    std::list<std::string> Evaluator::evaluate(query::Query query) {
        query::ResultCl resultcl = query.getResultCl();
        std::vector<query::AttrCompare> with = query.getWith();
        std::vector<std::shared_ptr<query::RelRef>> suchthat = query.getSuchthat();
        std::vector<query::Pattern> patterns = query.getPattern();

        optimizer::Optimizer optimizer = optimizer::Optimizer(suchthat, with, patterns);
        optimizer.optimize();

        while (optimizer.hasNextGroup()) {
            optimizer::ClauseGroup group = optimizer.nextGroup();
            ResultTable table = ResultTable();
            ClauseHandler handler = ClauseHandler(pkb, table);
            bool hasResult;
            if (group.noSyn()) {
                hasResult = handler.handleNoSynGroup(group);
            } else {
                hasResult = handler.handleGroup(group);
                intermediateTables.push_back(table);
            }

            if (!hasResult)
                return resultcl.isBoolean() ? std::list<std::string>{"FALSE"} : std::list<std::string>{};
        }

        std::vector<ResultTable> resultRelatedTables = findResultRelatedGroup(resultcl.getSynAsList());
        resultTable = resultRelatedTables.empty() ? ResultTable() : mergeGroupResults(resultRelatedTables);

        ClauseHandler handler = ClauseHandler(pkb, resultTable);
        handler.handleResultCl(resultcl);

        if (resultcl.isBoolean()) return std::list<std::string>{"TRUE"};
        return ResultProjector::projectResult(resultTable, resultcl);
    }
}  // namespace qps::evaluator
