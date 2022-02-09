#include "handleRelations.h"


//return true if relations has no synonym and relationship holds.
void RelationshipHandler::handleModifies(Modifies* mPtr) {
    Modifies m = *mPtr;
    EntRef modified = m.modified;
    StmtRef stmt = m.modifiesStmt;
    EntRefType entType = modified.getType();
    StmtRefType stmtType = stmt.getType();

    //get PKBResponse, then do cross join
    PKBResponse mResponse;
    //PKBRespone mResponse = pkb->getrelations();

    //If synonym not in table, insert else find order
    if (stmtType == StmtRefType::DECLARATION && entType == EntRefType::DECLARATION) {
        //If both synonyms not in map
        bool isFirstSynExists = tableRef.synExists(stmt.getDeclaration());
        bool isSecondSynExists = tableRef.synExists(modified.getDeclaration());
        if (!isFirstSynExists && !isSecondSynExists) {
            tableRef.insertSynLocationToLast(stmt.getDeclaration());
            tableRef.insertSynLocationToLast(modified.getDeclaration());
            tableRef.crossJoin(mResponse);
        } else if (isFirstSynExists && isSecondSynExists) {
            tableRef.innerJoin(mResponse, isFirstSynExists, isSecondSynExists,
                               std::vector<std::string>{stmt.getDeclaration(), modified.getDeclaration()});
        } else {
            if (isFirstSynExists) {
                tableRef.insertSynLocationToLast(modified.getDeclaration());
                tableRef.innerJoin(mResponse, isFirstSynExists, isSecondSynExists,
                                   std::vector<std::string>{stmt.getDeclaration(), modified.getDeclaration()});
            } else {
                tableRef.insertSynLocationToLast(stmt.getDeclaration());
                tableRef.innerJoin(mResponse, isFirstSynExists, isSecondSynExists,
                                   std::vector<std::string>{stmt.getDeclaration(), modified.getDeclaration()});
            }
        }
    } else if (stmtType == StmtRefType::DECLARATION) {
        bool isSynExists = tableRef.synExists(stmt.getDeclaration());
        if (isSynExists) {
            tableRef.innerJoin(mResponse, true, false, std::vector<std::string>{stmt.getDeclaration()});
        } else {
            tableRef.crossJoin(mResponse);
        }
    } else {
        bool isSynExists = tableRef.synExists(modified.getDeclaration());
        if (isSynExists) {
            tableRef.innerJoin(mResponse, false, true, std::vector<std::string>{modified.getDeclaration()});
        } else {
            tableRef.crossJoin(mResponse);
        }
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