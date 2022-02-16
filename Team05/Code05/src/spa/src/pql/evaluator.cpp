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

    PKBResponse Evaluator::getAll(query::DesignEntity type) {
        switch (type) {
            case query::DesignEntity::PROCEDURE: return pkb->getProcedures();
            case query::DesignEntity::CONSTANT: return pkb->getConstants();
            case query::DesignEntity::VARIABLE: return pkb->getVariables();
            case query::DesignEntity::STMT: return pkb->getStatements();
            default:
                StatementType sType = ClauseHandler::getStatementType(type);
                return pkb->getStatements(sType);
        }
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


    std::list<std::string> Evaluator::getListOfResult(ResultTable &table, std::string variable) {
        std::list<std::string> listResult{};
        if (table.getResult().empty()) {
            return listResult;
        }
        int synPos = table.getSynLocation(variable);
        std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> resultTable = table.getResult();
        for (auto field : resultTable) {
            listResult.push_back(PKBFieldToString(field[synPos]));
        }

        return listResult;
    }

    std::list<std::string> Evaluator::evaluate(query::Query query) {
        // std::unordered_map<std::string, DesignEntity> declarations = query.getDeclarations();
        std::vector<std::string> variable = query.getVariable();
        std::vector<std::shared_ptr<query::RelRef>> suchthat = query.getSuchthat();
        std::vector<std::shared_ptr<query::RelRef>> noSyn;
        std::vector<std::shared_ptr<query::RelRef>> hasSyn;
        std::vector<query::Pattern> pattern = query.getPattern();

        query::DesignEntity returnType = query.getDeclarationDesignEntity(variable[0]);
        ResultTable resultTable;
        ResultTable &tableRef = resultTable;
        query::Query &queryRef = query;


        if (!suchthat.empty()) {
            processSuchthat(suchthat, noSyn, hasSyn);
            ClauseHandler handler = ClauseHandler(pkb, tableRef, queryRef);
            if (!handler.handleNoSynClauses(noSyn)) return std::list<std::string>{};
            handler.handleSynClauses(hasSyn);
        }

        // After process suchthat and pattern if select variable not in result table, add all
        if (suchthat.empty() && pattern.empty() || !tableRef.synExists(variable[0])) {
            PKBResponse queryResult = getAll(returnType);
            std::vector<std::string> synonyms{variable[0]};
            tableRef.join(queryResult, synonyms);
        }

        return getListOfResult(tableRef, variable[0]);
    }
}  // namespace qps::evaluator
