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

    void ClauseHandler::handleSynRelRef(std::shared_ptr<query::RelRef> clause) {
        query::RelRef *relRefPtr = clause.get();
        std::vector<query::Declaration> declarations = relRefPtr->getDecs();
        std::vector<std::string> synonyms{};
        for (auto d : declarations) {
            synonyms.push_back(d.getSynonym());
        }
        std::vector<PKBField> fields = relRefPtr->getField();
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

        if (relRefPtr->getType() == query::RelRefType::AFFECTS || relRefPtr->getType() == query::RelRefType::AFFECTST)
            pkb->clearCache();
    }

    bool ClauseHandler::handleNoSynRelRef(const std::shared_ptr<query::RelRef>& noSynClauses) {
        query::RelRef *relRefPtr = noSynClauses.get();
        PKBField field1 = relRefPtr->getField()[0];
        PKBField field2 = relRefPtr->getField()[1];
        PKBRelationship relationship = PKBTypeMatcher::getPKBRelationship(relRefPtr->getType());
        if (!pkb->getRelationship(field1, field2, relationship).hasResult) return false;
        return true;
    }

    void ClauseHandler::handlePattern(query::Pattern pattern) {
        using sp::design_extractor::PatternParam;

        StatementType statementType = PKBTypeMatcher::getStatementType(pattern.getSynonymType());
        query::EntRef lhs = pattern.getEntRef();
        std::vector<std::string> synonyms{pattern.getSynonym()};
        if (lhs.isDeclaration()) synonyms.push_back(lhs.getDeclarationSynonym());

        std::optional<std::string> lhsParam = {};
        if (lhs.isVarName()) lhsParam = lhs.getVariableName();

        std::optional<std::string> rhsParam = {};
        bool isStrict = false;
        if (statementType == StatementType::Assignment) {
            query::ExpSpec exp = pattern.getExpression();
            if (exp.isPartialMatch() || exp.isFullMatch()) rhsParam = exp.getPattern();
            isStrict = exp.isFullMatch();
        }

        PKBResponse response = pkb->match(statementType, PatternParam(lhsParam), PatternParam(rhsParam, isStrict));
        if (!lhs.isDeclaration()) {
            response = selectDeclaredValue(response, true);
        }
        tableRef.insert(response, synonyms);
    }

    bool ClauseHandler::handleNoAttrRefWith(const query::AttrCompare& noAttrClause) {
            auto lhs = noAttrClause.lhs;
            auto rhs = noAttrClause.rhs;

        if (lhs.isString() && rhs.isString() && lhs.getString() != rhs.getString()) return false;
        else if (lhs.isNumber() && rhs.isNumber() && lhs.getNumber() != rhs.getNumber()) return false;

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

    void ClauseHandler::handleAttrRefWith(query::AttrCompare attrClause) {
        auto lhs = attrClause.lhs;
        auto rhs = attrClause.rhs;
        if (lhs.isAttrRef() && rhs.isAttrRef()) {
            handleTwoAttrRef(lhs.getAttrRef(), rhs.getAttrRef());
        } else {
            if (!lhs.isAttrRef())  handleOneAttrRef(rhs.getAttrRef(), lhs);
            else
                handleOneAttrRef(lhs.getAttrRef(), rhs);
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


    bool ClauseHandler::handleGroup(optimizer::ClauseGroup group) {
        while (group.hasNextClause()) {
            optimizer::OrderedClause clause = group.nextClause();
            if (clause.isSuchThat()) {
                handleSynRelRef(clause.getSuchThat());
            } else if (clause.isWith()) {
                handleAttrRefWith(clause.getWith());
            } else {
                handlePattern(clause.getPattern());
            }

            if (!tableRef.hasResult()) return false;
        }
        return true;
    }

    bool ClauseHandler::handleNoSynGroup(optimizer::ClauseGroup group) {
        while (group.hasNextClause()) {
            optimizer::OrderedClause clause = group.nextClause();
            bool isHold;
            if (clause.isSuchThat()) {
                isHold = handleNoSynRelRef(clause.getSuchThat());
            } else {
                isHold = handleNoAttrRefWith(clause.getWith());
            }
            if (!isHold) return false;
        }
        return true;
    }
}  // namespace qps::evaluator
