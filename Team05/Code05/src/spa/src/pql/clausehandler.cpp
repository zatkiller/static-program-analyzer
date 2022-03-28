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

    void ClauseHandler::processStmtField(std::vector<PKBField> &fields, std::vector<query::Declaration> declarations) {
        if (fields[0].entityType == PKBEntityType::STATEMENT && fields[0].fieldType == PKBFieldType::DECLARATION) {
            StatementType sType = PKBTypeMatcher::getStatementType(declarations[0].getType());
            fields[0] = PKBField::createDeclaration(sType);
        }
        if (fields[1].entityType == PKBEntityType::STATEMENT && fields[1].fieldType == PKBFieldType::DECLARATION) {
            query::Declaration d = declarations.size() == 2 ? declarations[1] : declarations[0];
            StatementType sType = PKBTypeMatcher::getStatementType(d.getType());
            fields[1] = PKBField::createDeclaration(sType);
        }
    }

    PKBResponse ClauseHandler::selectDeclaredValue(PKBResponse& response, bool isFirstSyn) {
        int erasePos = isFirstSyn ? 1 : 0;
        if (!response.hasResult) return response;
        PKBResponse newResponse;
        VectorResponse res;
        auto *ptr = std::get_if<VectorResponse>(&response.res);
        for (auto v : *ptr) {
            std::vector<PKBField> newrecord = v;
            newrecord.erase(newrecord.begin() + erasePos);
            res.insert(newrecord);
        }
        newResponse = PKBResponse{response.hasResult, Response {res}};
        return newResponse;
    }

    void ClauseHandler::filterPKBResponse(PKBResponse& response) {
        if (!response.hasResult) return;
        auto *ptr =
                std::get_if<VectorResponse>(&response.res);
        VectorResponse copy = *ptr;
        for (auto v : copy) {
            if (v[0] == v[1]) {
                auto newRecord = v;
                newRecord.pop_back();
                ptr->insert(newRecord);
            }
            ptr->erase(v);
        }
    }

    void ClauseHandler::handleSynClauses(std::vector<std::shared_ptr<query::RelRef>> clauses) {
        for (auto c : clauses) {
            query::RelRef *relRefPtr = c.get();
            std::vector<query::Declaration> declarations = relRefPtr->getSyns();
            std::vector<std::string> synonyms{};
            for (auto d : declarations) {
                synonyms.push_back(d.getSynonym());
            }
            std::vector<PKBField> fields = relRefPtr->getField();
            processStmtField(fields, declarations);
            PKBResponse response = pkb->getRelationship(fields[0], fields[1],
                                                        PKBTypeMatcher::getPKBRelationship(relRefPtr->getType()));
            bool isFirstSyn = fields[0].fieldType == PKBFieldType::DECLARATION;
            bool isSecondSyn = fields[1].fieldType == PKBFieldType::DECLARATION;
            if (!isFirstSyn || !isSecondSyn) {
                response = selectDeclaredValue(response, isFirstSyn);
            } else if (synonyms[0] == synonyms[1]) {
                synonyms.pop_back();
                filterPKBResponse(response);
            }
            tableRef.insert(response, synonyms);
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
            StatementType statementType = PKBTypeMatcher::getStatementType(pattern.getSynonymType());
            query::EntRef lhs = pattern.getEntRef();
            std::vector<std::string> synonyms{pattern.getSynonym()};
            if (lhs.isDeclaration()) synonyms.push_back(lhs.getDeclarationSynonym());
            query::ExpSpec exp = pattern.getExpression();

            std::optional<std::string> lhsParam = {};
            if (lhs.isVarName()) lhsParam = lhs.getVariableName();

            std::optional<std::string> rhsParam = {};
            if (exp.isPartialMatch() || exp.isFullMatch()) rhsParam = exp.getPattern();

            bool isStrict = exp.isFullMatch();
            PKBResponse response = pkb->match(statementType, lhsParam, rhsParam, isStrict);
            if (!lhs.isDeclaration()) {
                response = selectDeclaredValue(response, true);
            }
            tableRef.insert(response, synonyms);
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
        PKBResponse lhsResult = getAll(lhs.getDeclarationType());
        PKBResponse rhsResult = getAll(rhs.getDeclarationType());
        PKBResponse newResponse;
        auto lhsPtr = std::get_if<SingleResponse>(&lhsResult.res);
        auto rhsPtr = std::get_if<SingleResponse>(&rhsResult.res);
        if (lhs.getDeclarationSynonym() == rhs.getDeclarationSynonym()) {
            tableRef.insert(lhsResult, std::vector<std::string>{lhs.getDeclarationSynonym()});
            return;
        }
        if (lhs.getAttrName() == query::AttrName::PROCNAME || lhs.getAttrName() == query::AttrName::VARNAME) {
            newResponse = twoAttrMerge<std::string>(*lhsPtr, *rhsPtr);
        } else {
            newResponse = twoAttrMerge<int>(*lhsPtr, *rhsPtr);
        }
        tableRef.insert(newResponse,
                        std::vector<std::string>{lhs.getDeclarationSynonym(), rhs.getDeclarationSynonym()});
    }

    void ClauseHandler::handleOneAttrRef(query::AttrRef attr, query::AttrCompareRef concrete) {
        PKBResponse attrResult = getAll(attr.getDeclarationType());
        if (concrete.isString()) attrResult = filterAttrValue<std::string>(attrResult, concrete.getString());
        if (concrete.isNumber()) attrResult = filterAttrValue<int>(attrResult, concrete.getNumber());
        tableRef.insert(attrResult, std::vector<std::string>{attr.getDeclarationSynonym()});
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

    void ClauseHandler::handleResultCl(query::ResultCl resultCl) {
        std::vector<query::Elem> tuple = resultCl.getTuple();
        for (auto elem : tuple) {
            query::Declaration dec;
            if (elem.isDeclaration()) dec = elem.getDeclaration();
            else
                dec = elem.getAttrRef().getDeclaration();
            if (!tableRef.synExists(dec.getSynonym())) {
                PKBResponse r = getAll(dec.getType());
                tableRef.insert(r, std::vector<std::string>{dec.getSynonym()});
            }
        }
    }
}  // namespace qps::evaluator
