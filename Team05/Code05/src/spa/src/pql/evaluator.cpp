#include "evaluator.h"

#define DEBUG Logger(Level::DEBUG) << "evaluator.cpp "

namespace qps::evaluator {

    SelectElemInfo SelectElemInfo::ofDeclaration(int columnNo) {
        SelectElemInfo e;
        e.columnNo = columnNo;
        e.isAttr = false;
        return e;
    }

    SelectElemInfo SelectElemInfo::ofAttr(int columnNo, query::AttrName attrName) {
        SelectElemInfo e;
        e.columnNo = columnNo;
        e.attrName = std::move(attrName);
        e.isAttr = true;
        return e;
    }

    std::string Evaluator::PKBFieldToString(PKBField pkbField) {
        std::string res;
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

    std::string Evaluator::PKBFieldAttrToString(PKBField pkbField, query::AttrName attrName) {
        std::string res;
        if (attrName == query::AttrName::PROCNAME || attrName == query::AttrName::VARNAME) {
            res = ClauseHandler::getPKBFieldAttr<std::string>(pkbField);
        } else {
            res = std::to_string(ClauseHandler::getPKBFieldAttr<int>(pkbField));
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

    std::list<std::string> Evaluator::getListOfResult(ResultTable &table, query::ResultCl resultCl) {
        std::list<std::string> listResult{};
        if (table.hasResult()) {
            return listResult;
        }
        std::vector<query::Elem> tuple = resultCl.getTuple();
        std::vector<SelectElemInfo> elem{};
        for (auto e : tuple) {
            SelectElemInfo elemInfo;
            if (e.isDeclaration())
                elemInfo = SelectElemInfo::ofDeclaration(table.getSynLocation(e.getDeclaration().getSynonym()));
            else
                elemInfo = SelectElemInfo::ofAttr(table.getSynLocation(e.getAttrRef().getDeclarationSynonym()),
                                e.getAttrRef().getAttrName());
            elem.push_back(elemInfo);
        }
        std::unordered_set<std::string> resultSet;
        for (auto record : table.getTable()) {
            std::string result;
            for (auto e : elem) {
                PKBField fld = record[e.columnNo];
                result += (e.isAttr ? PKBFieldAttrToString(fld, e.attrName)
                        : PKBFieldToString(fld)) + " ";
            }
            result.erase(std::find_if(result.rbegin(), result.rend(), [](unsigned char ch) {
                return !std::isspace(ch);}).base(), result.end());
            if (resultSet.find(result) == resultSet.end()) {
                listResult.push_back(result);
                resultSet.insert(result);
            }
        }

        return listResult;
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

        return getListOfResult(resultTable, resultcl);
    }
}  // namespace qps::evaluator
