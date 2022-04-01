#include "evaluator.h"

#define DEBUG Logger(Level::DEBUG) << "evaluator.cpp "

namespace qps::evaluator {
    void Evaluator::processSuchthat(std::vector<std::shared_ptr<query::RelRef>> clauses,
                                    std::vector<std::shared_ptr<query::RelRef>> &noSyn,
                                    std::vector<std::shared_ptr<query::RelRef>> &hasSyn) {
        for (auto r : clauses) {
            query::RelRef *relRefPtr = r.get();
            if (relRefPtr->getDecs().empty()) {
                noSyn.push_back(r);
            } else {
                hasSyn.push_back(r);
            }
        }
    }

    void Evaluator::processWith(std::vector<query::AttrCompare> withClauses,
                     std::vector<query::AttrCompare> &noAttrRef, std::vector<query::AttrCompare> &hasAttrRef) {
        for (auto clause : withClauses) {
            if (!clause.lhs.isAttrRef() && !clause.rhs.isAttrRef()) {
                noAttrRef.push_back(clause);
            } else {
                hasAttrRef.push_back(clause);
            }
        }
    }

    std::list<std::string> Evaluator::evaluate(query::Query query) {
        // std::unordered_map<std::string, DesignEntity> declarations = query.getDeclarations();
        query::ResultCl resultcl = query.getResultCl();
        std::vector<query::AttrCompare> with = query.getWith();
        std::vector<query::AttrCompare> noAttrRef;
        std::vector<query::AttrCompare> hasAttrRef;

        std::vector<std::shared_ptr<query::RelRef>> suchthat = query.getSuchthat();
        std::vector<std::shared_ptr<query::RelRef>> noSyn;
        std::vector<std::shared_ptr<query::RelRef>> hasSyn;

        std::vector<query::Pattern> patterns = query.getPattern();

        ClauseHandler handler = ClauseHandler(pkb, resultTable);

        if (!with.empty()) {
            processWith(with, noAttrRef, hasAttrRef);
            if (!handler.handleNoAttrRefWith(noAttrRef))
                return resultcl.isBoolean() ? std::list<std::string>{"FALSE"} : std::list<std::string>{};
            handler.handleAttrRefWith(hasAttrRef);
        }

        if (!suchthat.empty()) {
            processSuchthat(suchthat, noSyn, hasSyn);
            if (!handler.handleNoSynClauses(noSyn))
                return resultcl.isBoolean() ? std::list<std::string>{"FALSE"} :std::list<std::string>{};
            handler.handleSynClauses(hasSyn);
        }

        if (!patterns.empty()) {
            handler.handlePatterns(patterns);
        }

        handler.handleResultCl(resultcl);
        if (resultcl.isBoolean()) {
            if (with.empty() && suchthat.empty() && patterns.empty()) return std::list<std::string>{"TRUE"};
            return resultTable.hasResult() ? std::list<std::string>{"FALSE"} : std::list<std::string>{"TRUE"};
        }

        return ResultProjector::projectResult(resultTable, resultcl);
//        return getListOfResult(resultTable, resultcl);
    }
}  // namespace qps::evaluator
