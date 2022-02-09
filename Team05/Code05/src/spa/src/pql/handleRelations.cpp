#include "handleRelations.h"


//return true if relations has no synonym and relationship holds.
void ClauseHandler::handleModifies(Modifies* mPtr) {
    Modifies m = *mPtr;
    EntRef modified = m.modified;
    StmtRef stmt = m.modifiesStmt;
    std::vector<std::string> synonyms;
    if (stmt.isDeclaration()){
        synonyms.push_back(stmt.getDeclaration());
    }
    if(modified.isDeclaration()) {
        synonyms.push_back(modified.getDeclaration());
    }
    //get PKBResponse, then do result join
    PKBResponse mResponse;
    //PKBRespone mResponse = pkb->getrelations();
    tableRef.join(mResponse, synonyms);

}

void ClauseHandler::handleUses(Uses* uPtr) {
    Uses u = *uPtr;
    StmtRef stmt = u.useStmt;
    EntRef used = u.used;
    std::vector<std::string> synonyms;
    if (stmt.isDeclaration()){
        synonyms.push_back(stmt.getDeclaration());
    }
    if (used.isDeclaration()) {
        synonyms.push_back(used.getDeclaration());
    }
    //get PKBResponse
    PKBResponse uResponse;
    // API call here
    tableRef.join(uResponse, synonyms);

}

void ClauseHandler::handleSynClauses(std::vector<std::shared_ptr<RelRef>> clauses) {
    for (auto c : clauses) {
        RelRef* relRefPtr = c.get();
        RelRefType type = relRefPtr->getType();
        if (type == RelRefType::MODIFIESS) {
            handleModifies(dynamic_cast<Modifies*>(relRefPtr));
        } else if (type == RelRefType::USESS) {
            handleUses(dynamic_cast<Uses*>(relRefPtr));
        }
    }
}

bool ClauseHandler::evaluateNoSynClauses(std::vector<std::shared_ptr<RelRef>> noSynClauses) {
    bool hold = true;
    for (auto clause : noSynClauses) {
        RelRef* relRefPtr = clause.get();
        if (relRefPtr->getType() == RelRefType::MODIFIESS) {
            Modifies* mPtr= dynamic_cast<Modifies*>(relRefPtr);
            EntRef modified = mPtr->modified;
            StmtRef stmt = mPtr->modifiesStmt;
            //add method enfRefGeneratePKBField and stmtRefGeneratePKBField
//            PKBField modifiedField;
//            PKBField Modified;
//API call
//        hold = hold && pkb->isRelationshipPresent(stmtPKBField, modified, PKBRelationship::MODIFIES);
        } else if (relRefPtr->getType() == RelRefType::MODIFIESS) {
            Uses* uPtr = dynamic_cast<Uses*>(relRefPtr);
            EntRef used = uPtr->used;
            StmtRef stmt = uPtr->useStmt;
            // create PKBField, make API call
        }
    }
    return hold;
}