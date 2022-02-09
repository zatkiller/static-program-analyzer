#include "handleRelations.h"


//return true if relations has no synonym and relationship holds.
void RelationshipHandler::handleModifies(Modifies* mPtr) {
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
    //get PKBResponse, then do cross join
    PKBResponse mResponse;
    //PKBRespone mResponse = pkb->getrelations();
    if (mResponse.hasResult) {
        tableRef.join(mResponse, synonyms);
    }
}

void RelationshipHandler::handleSynClauses(std::vector<std::shared_ptr<RelRef>> clauses) {
    for (auto c : clauses) {
        RelRef* relRefPtr = c.get();
        RelRefType type = relRefPtr->getType();
        if (type == RelRefType::MODIFIESS) {
            handleModifies(dynamic_cast<Modifies*>(relRefPtr));
        }
    }
}