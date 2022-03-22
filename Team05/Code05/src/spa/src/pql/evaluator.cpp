#include "evaluator.h"

#define DEBUG Logger(Level::DEBUG) << "evaluator.cpp "

namespace qps::evaluator {

    std::string Evaluator::PKBFieldToString(PKBField pkbField) {
        std::string res = "";
        if (pkbField.entityType == PKBEntityType::STATEMENT) {
            int lineNo = std::get<STMT_LO>(pkbField.content).statementNum;
            res = std::to_string(lineNo);
        } else if (pkbField.entityType == PKBEntityType::CONST) {
            int c = std::get<CONST>(pkbField.content);
            res = std::to_string(c);
        } else if (pkbField.entityType == PKBEntityType::VARIABLE) {
            res = std::get<VAR_NAME>(pkbField.content).name;
        } else if (pkbField.entityType == PKBEntityType::PROCEDURE) {
            res = std::get<PROC_NAME>(pkbField.content).name;
        }
        return res;
    }

    void Evaluator::processSuchthat(std::vector<std::shared_ptr<query::RelRef>> clauses,
                                    std::vector<std::shared_ptr<query::RelRef>> &noSyn,
                                    std::vector<std::shared_ptr<query::RelRef>> &hasSyn) {
        for (auto r : clauses) {
            query::RelRef *relRefPtr = r.get();
            if (relRefPtr->getSyns().empty()) {
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

    std::list<std::string> Evaluator::getListOfResult(ResultTable &table, std::string variable) {
        std::list<std::string> listResult{};
        if (table.getResult().empty()) {
            return listResult;
        }
        int synPos = table.getSynLocation(variable);
        std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> resultTable = table.getResult();
        std::unordered_set<std::string> resultSet;
        for (auto field : resultTable) {
            if (resultSet.find(PKBFieldToString(field[synPos])) == resultSet.end()) {
                listResult.push_back(PKBFieldToString(field[synPos]));
                resultSet.insert(PKBFieldToString(field[synPos]));
            }
        }

        return listResult;
    }

    std::list<std::string> Evaluator::evaluate(query::Query query) {
        // std::unordered_map<std::string, DesignEntity> declarations = query.getDeclarations();
        std::vector<std::string> variable = query.getVariable();
        std::vector<query::AttrCompare> with = query.getWith();
        std::vector<query::AttrCompare> noAttrRef;
        std::vector<query::AttrCompare> hasAttrRef;

        std::vector<std::shared_ptr<query::RelRef>> suchthat = query.getSuchthat();
        std::vector<std::shared_ptr<query::RelRef>> noSyn;
        std::vector<std::shared_ptr<query::RelRef>> hasSyn;

        std::vector<query::Pattern> patterns = query.getPattern();

        query::DesignEntity returnType = query.getDeclarationDesignEntity(variable[0]);
        ClauseHandler handler = ClauseHandler(pkb, resultTable, query);

        if (!with.empty()) {
            processWith(with, noAttrRef, hasAttrRef);
            if (!handler.handleNoAttrRefWith(noAttrRef)) return std::list<std::string>{};
            handler.handleAttrRefWith(hasAttrRef);
        }

        if (!suchthat.empty()) {
            processSuchthat(suchthat, noSyn, hasSyn);
            if (!handler.handleNoSynClauses(noSyn)) return std::list<std::string>{};
            handler.handleSynClauses(hasSyn);
        }

        if (!patterns.empty()) {
            handler.handlePatterns(patterns);
        }

        // After process suchthat and pattern if select variable not in result table, add all
        if ((suchthat.empty() && patterns.empty()) || !resultTable.synExists(variable[0])) {
            PKBResponse queryResult = handler.getAll(returnType);
            std::vector<std::string> synonyms{variable[0]};
            resultTable.insert(queryResult, synonyms);
        }

        return getListOfResult(resultTable, variable[0]);
    }
}  // namespace qps::evaluator
