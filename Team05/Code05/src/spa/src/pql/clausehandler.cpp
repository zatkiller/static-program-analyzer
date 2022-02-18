#include "clausehandler.h"

namespace qps::evaluator {
    PKBRelationship ClauseHandler::getPKBRelationship(query::RelRefType r) {
        switch (r) {
            case query::RelRefType::MODIFIESS: return PKBRelationship::MODIFIES;
            case query::RelRefType::USESS: return PKBRelationship::USES;
            case query::RelRefType::FOLLOWS: return PKBRelationship::FOLLOWS;
            case query::RelRefType::FOLLOWST: return PKBRelationship::FOLLOWST;
            case query::RelRefType::PARENT: return PKBRelationship::PARENT;
//            case query::RelRefType::PARENTT: return PKBRelationship::PARENTT;
        }
    }

    StatementType ClauseHandler::getStatementType(query::DesignEntity d) {
        switch (d) {
            case query::DesignEntity::STMT: return StatementType::All;
            case query::DesignEntity::ASSIGN: return StatementType::Assignment;
            case query::DesignEntity::WHILE: return StatementType::While;
            case query::DesignEntity::IF: return StatementType::If;
            case query::DesignEntity::READ: return StatementType::Read;
            case query::DesignEntity::PRINT: return StatementType::Print;
            case query::DesignEntity::CALL: return StatementType::Call;
        }
    }

    void ClauseHandler::processStmtField(std::vector<PKBField> &fields, std::vector<std::string> synonyms) {
        if (fields[0].entityType == PKBEntityType::STATEMENT && fields[0].fieldType == PKBFieldType::DECLARATION) {
            StatementType sType = getStatementType(query.getDeclarationDesignEntity(synonyms[0]));
            fields[0] = PKBField::createDeclaration(sType);
        }
        if (fields[1].entityType == PKBEntityType::STATEMENT && fields[1].fieldType == PKBFieldType::DECLARATION) {
            std::string name = synonyms.size() == 2 ? synonyms[1] : synonyms[0];
            StatementType sType = getStatementType(query.getDeclarationDesignEntity(name));
            fields[1] = PKBField::createDeclaration(sType);
        }
    }

    PKBResponse ClauseHandler::selectDeclarationValue(PKBResponse& response, bool isFirstDec) {
        int erasePos = isFirstDec ? 1 : 0;
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
                                                        getPKBRelationship(relRefPtr->getType()));
            bool isFirstDec = fields[0].fieldType == PKBFieldType::DECLARATION;
            bool isSecondDec = fields[1].fieldType == PKBFieldType::DECLARATION;
            if (!isFirstDec || !isSecondDec) {
                response = selectDeclarationValue(response, isFirstDec);
            } else if (isFirstDec && isSecondDec) {
                response = filterPKBResponse(response, synonyms[0] == synonyms[1]);
            }
            tableRef.join(response, synonyms);
        }
    }

    bool ClauseHandler::handleNoSynClauses(std::vector<std::shared_ptr<query::RelRef>> noSynClauses) {
        for (auto clause : noSynClauses) {
            query::RelRef *relRefPtr = clause.get();
            PKBField field1 = relRefPtr->getField()[0];
            PKBField field2 = relRefPtr->getField()[1];
            PKBRelationship relationship = getPKBRelationship(relRefPtr->getType());
            if (!pkb->getRelationship(field1, field2, relationship).hasResult) return false;
//            if (!pkb->isRelationshipPresent(field1, field2, relationship)) return false;
        }
        return true;
    }

}  // namespace qps::evaluator
