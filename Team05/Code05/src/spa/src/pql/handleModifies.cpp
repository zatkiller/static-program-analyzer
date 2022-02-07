#include "handleModifies.h"


void handleModifies(std::shared_ptr<RelRef> clause) {
    std::shared_ptr<Modifies> mPtr = std::dynamic_pointer_cast<Modifies>(clause);
    Modifies* modifiesPtr = mPtr.get();
    Modifies m = *modifiesPtr;
    EntRef modified = m.modified;
    StmtRef stmt = m.modifiesStmt;
    EntRefType entType = modified.getType();
    StmtRefType stmtType = stmt.getType();

    if ((stmt.isWildcard() || stmt.isLineNo()) && (modified.isWildcard() || modified.isVarName())) {
        //include PKB files
        STMT_LO stmtLineNo;
        stmtLineNo.statementNum = stmt.getLineNo();
        Content stmtContent{};
        if (stmt.isLineNo()) {
            stmtContent = Content {STMT_LO{stmt.getLineNo()}};
        }
        Content varContent{};
        if (modified.isVarName()) {
            varContent = Content {VAR_NAME{modified.getVariableName()}};
        }
        //isConcrete?
        PKBField stmtPKBField = PKBField(PKBType::STATEMENT, stmt.isWildcard() ? true : false, stmtContent);
        PKBField moidifiedPKBField = PKBField(PKBType::VARIABLE, false, varContent);
        bool hasRelationships;
//API call
//        hasRelationships = isRelationshipPresent(stmtPKBField, modified, PKBRelationship::MODIFIES);
        if (hasRelationships) {

        }
    } else if (stmt.isDeclaration() && modified.isDeclaration()) {

    } else {

    }
}