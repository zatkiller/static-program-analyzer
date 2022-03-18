#include "clausehandler.h"

namespace qps::evaluator {
    PKBResponse ClauseHandler::getAll(query::DesignEntity type) {
        switch (type) {
            case query::DesignEntity::PROCEDURE: return pkb->getProcedures();
            case query::DesignEntity::CONSTANT: return pkb->getConstants();
            case query::DesignEntity::VARIABLE: return pkb->getVariables();
            case query::DesignEntity::STMT: return pkb->getStatements();
            default:
                StatementType sType = PKBTypeMatcher::getStatementType(type);
                return pkb->getStatements(sType);
        }
    }

    void ClauseHandler::processStmtField(std::vector<PKBField> &fields, std::vector<std::string> synonyms) {
        if (fields[0].entityType == PKBEntityType::STATEMENT && fields[0].fieldType == PKBFieldType::DECLARATION) {
            StatementType sType = PKBTypeMatcher::getStatementType(query.getDeclarationDesignEntity(synonyms[0]));
            fields[0] = PKBField::createDeclaration(sType);
        }
        if (fields[1].entityType == PKBEntityType::STATEMENT && fields[1].fieldType == PKBFieldType::DECLARATION) {
            std::string name = synonyms.size() == 2 ? synonyms[1] : synonyms[0];
            StatementType sType = PKBTypeMatcher::getStatementType(query.getDeclarationDesignEntity(name));
            fields[1] = PKBField::createDeclaration(sType);
        }
    }

    PKBResponse ClauseHandler::selectDeclaredValue(PKBResponse& response, bool isFirstSyn) {
        int erasePos = isFirstSyn ? 1 : 0;
        if (!response.hasResult) return response;
        PKBResponse newResponse;
        std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> res;
        auto *ptr =
                std::get_if<std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>>(&response.res);
        for (auto v : *ptr) {
            std::vector<PKBField> newrecord = v;
            newrecord.erase(newrecord.begin() + erasePos);
            res.insert(newrecord);
        }
        newResponse = PKBResponse{response.hasResult, Response {res}};
        return newResponse;
    }

    PKBResponse ClauseHandler::filterPKBResponse(PKBResponse& response, bool isSame) {
        if (!response.hasResult) return response;
        PKBResponse newResponse;
        std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash> res;
        auto *ptr =
                std::get_if<std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>>(&response.res);
        for (auto v : *ptr) {
            std::vector<PKBField> newrecord = v;
            if ((newrecord[0] == newrecord[1]) == isSame) res.insert(newrecord);
        }
        newResponse = PKBResponse{response.hasResult, Response {res}};
        return newResponse;
    }

    void ClauseHandler::handleSynClauses(std::vector<std::shared_ptr<query::RelRef>> clauses) {
        for (auto c : clauses) {
            query::RelRef *relRefPtr = c.get();
            std::vector<std::string> synonyms = relRefPtr->getSyns();
            std::vector<PKBField> fields = relRefPtr->getField();
            processStmtField(fields, synonyms);
            PKBResponse response = pkb->getRelationship(fields[0], fields[1],
                                                        PKBTypeMatcher::getPKBRelationship(relRefPtr->getType()));
            bool isFirstSyn = fields[0].fieldType == PKBFieldType::DECLARATION;
            bool isSecondSyn = fields[1].fieldType == PKBFieldType::DECLARATION;
            if (!isFirstSyn || !isSecondSyn) {
                response = selectDeclaredValue(response, isFirstSyn);
            } else {
                response = filterPKBResponse(response, synonyms[0] == synonyms[1]);
            }
            tableRef.join(response, synonyms);
        }
    }

    bool ClauseHandler::handleNoSynClauses(const std::vector<std::shared_ptr<query::RelRef>>& noSynClauses) {
        for (const auto& clause : noSynClauses) {
            query::RelRef *relRefPtr = clause.get();
            PKBField field1 = relRefPtr->getField()[0];
            PKBField field2 = relRefPtr->getField()[1];
            PKBRelationship relationship = PKBTypeMatcher::getPKBRelationship(relRefPtr->getType());
            if (!pkb->getRelationship(field1, field2, relationship).hasResult) return false;
        }
        return true;
    }

    void ClauseHandler::handlePatterns(std::vector<query::Pattern> patterns) {
        for (auto pattern : patterns) {
            StatementType statementType = PKBTypeMatcher::getStatementType(
                    query.getDeclarationDesignEntity(pattern.getSynonym()));
            query::EntRef lhs = pattern.getEntRef();
            std::vector<std::string> synonyms{pattern.getSynonym()};
            if (lhs.isDeclaration()) synonyms.push_back(lhs.getDeclaration());
            query::ExpSpec exp = pattern.getExpression();

            std::optional<std::string> lhsParam = {};
            if (lhs.isVarName()) lhsParam = lhs.getVariableName();

            std::optional<std::string> rhsParam = {};
            if (exp.isPartialMatch()) rhsParam = exp.getPattern();

            PKBResponse response = pkb->match(statementType, lhsParam, rhsParam);
            if (!lhs.isDeclaration()) {
                response = selectDeclaredValue(response, true);
            }
            tableRef.join(response, synonyms);
        }
    }

    bool ClauseHandler::handleNoAttrRefWith(const std::vector<query::AttrCompare>& noAttrClauses) {
        for (const auto& clause : noAttrClauses) {
            auto lhs = clause.lhs;
            auto rhs = clause.rhs;

            if (lhs.isString() && rhs.isString() && lhs.getString() == rhs.getString()) continue;
            else if (lhs.isNumber() && rhs.isNumber() && lhs.getNumber() == rhs.getNumber()) continue;
            else
                return false;
        }
        return true;
    }

    void ClauseHandler::handleTwoAttrRef(query::AttrRef lhs, query::AttrRef rhs) {
        PKBResponse lhsResult = getAll(lhs.declarationType);
        PKBResponse rhsResult = getAll(rhs.declarationType);
        PKBResponse newResponse;
        auto lhsPtr = std::get_if<SingleResponse>(&lhsResult.res);
        auto rhsPtr = std::get_if<SingleResponse>(&rhsResult.res);
        if (lhs.declaration == rhs.declaration) {
            tableRef.join(lhsResult, std::vector<std::string>{lhs.declaration});
            return;
        }
        if (lhs.attrName == query::AttrName::PROCNAME || lhs.attrName == query::AttrName::VARNAME) {
            newResponse = twoAttrMerge<std::string>(*lhsPtr, *rhsPtr);
        } else {
            newResponse = twoAttrMerge<int>(*lhsPtr, *rhsPtr);
        }
        tableRef.join(newResponse, std::vector<std::string>{lhs.declaration, rhs.declaration});
    }

    void ClauseHandler::handleOneAttrRef(query::AttrRef attr, query::AttrCompareRef concrete) {
        PKBResponse attrResult = getAll(attr.declarationType);
        if (concrete.isString()) attrResult = filterAttrValue<std::string>(attrResult, concrete.getString());
        if (concrete.isNumber()) attrResult = filterAttrValue<int>(attrResult, concrete.getNumber());
        tableRef.join(attrResult, std::vector<std::string>{attr.declaration});
    }

    void ClauseHandler::handleAttrRefWith(std::vector<query::AttrCompare> attrClauses) {
        for (auto clause : attrClauses) {
            auto lhs = clause.lhs;
            auto rhs = clause.rhs;
            if (lhs.isAttrRef() && rhs.isAttrRef()) {
                handleTwoAttrRef(lhs.getAttrRef(), rhs.getAttrRef());
            } else {
                if (!lhs.isAttrRef())  handleOneAttrRef(rhs.getAttrRef(), lhs);
                else
                    handleOneAttrRef(lhs.getAttrRef(), rhs);
            }
        }
    }
}  // namespace qps::evaluator
