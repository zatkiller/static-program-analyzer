#include "clausehandler.h"

StatementType ClauseHandler::getStatementType(DesignEntity d) {
    if (d == DesignEntity::STMT) {
        return StatementType::Statement;
    } else if (d == DesignEntity::ASSIGN) {
        return StatementType::Assignment;
    } else if (d == DesignEntity::WHILE) {
        return StatementType::While;
    } else if (d == DesignEntity::IF) {
        return StatementType::If;
    } else if (d == DesignEntity::READ) {
        return StatementType::Read;
    } else if (d == DesignEntity::PRINT) {
        return StatementType::Print;
    } else if (d == DesignEntity::CALL) {
        return StatementType::Call;
    }
}

PKBField ClauseHandler::wrapStmtRef(StmtRef stmtRef) {
    PKBField stmtField;
    if (stmtRef.isLineNo()) {
        stmtField = PKBField::createConcrete(STMT_LO{stmtRef.getLineNo()});
    } else if (stmtRef.isWildcard()) {
        stmtField = PKBField::createWildcard(PKBEntityType::STATEMENT);
    } else if (stmtRef.isDeclaration()) {
        stmtField = PKBField::createDeclaration(getStatementType(query.getDeclarationDesignEntity(stmtRef.getDeclaration())));
    }
    return stmtField;
}

PKBField ClauseHandler::wrapEntRef(EntRef entRef) {
    PKBField entField;
    if (entRef.isVarName()) {
        entField = PKBField::createConcrete(VAR_NAME{entRef.getVariableName()});
    } else if (entRef.isWildcard()) {
        entField = PKBField::createWildcard(PKBEntityType::VARIABLE);
    } else if (entRef.isDeclaration()) {
        entField = PKBField::createDeclaration(PKBEntityType::VARIABLE);
    }
    return entField;
}

void ClauseHandler::handleSynClauses(std::vector<std::shared_ptr<RelRef>> clauses) {
    for (auto c : clauses) {
        RelRef* relRefPtr = c.get();
        RelRefType type = relRefPtr->getType();
        if (type == RelRefType::MODIFIESS) {
            evaluateRelationships(dynamic_cast<Modifies*>(relRefPtr), &Modifies::modifiesStmt,
                                &Modifies::modified, PKBRelationship::MODIFIES);
        } else if (type == RelRefType::USESS) {
            evaluateRelationships(dynamic_cast<Uses*>(relRefPtr), &Uses::useStmt,
                                &Uses::used, PKBRelationship::USES);
        } else if (type == RelRefType::FOLLOWS) {
            evaluateRelationships(dynamic_cast<Follows*>(relRefPtr), &Follows::follower,
                                &Follows::followed, PKBRelationship::FOLLOWS);
        } else if (type == RelRefType::FOLLOWST) {
            evaluateRelationships(dynamic_cast<FollowsT*>(relRefPtr), &FollowsT::follower,
                                &FollowsT::transitiveFollowed, PKBRelationship::FOLLOWST);
        } else if (type == RelRefType::PARENT) {
            evaluateRelationships(dynamic_cast<Parent*>(relRefPtr), &Parent::parent,
                                &Parent::child, PKBRelationship::PARENT);
        } else if (type == RelRefType::PARENTT) {
            evaluateRelationships(dynamic_cast<ParentT*>(relRefPtr), &ParentT::parent,
                                &ParentT::transitiveChild, PKBRelationship::PARENTT);
        }
    }
}

bool ClauseHandler::handleNoSynClauses(std::vector<std::shared_ptr<RelRef>> noSynClauses) {
    for (auto clause : noSynClauses) {
        RelRef* relRefPtr = clause.get();
        if (relRefPtr->getType() == RelRefType::MODIFIESS) {
            Modifies* mPtr= dynamic_cast<Modifies*>(relRefPtr);
            if (!evaluateNoSynRelRef(PKBRelationship::MODIFIES, mPtr, &Modifies::modifiesStmt,
                                     &Modifies::modified)) return false;
        } else if (relRefPtr->getType() == RelRefType::USESS) {
            Uses* uPtr = dynamic_cast<Uses*>(relRefPtr);
            if (!evaluateNoSynRelRef(PKBRelationship::USES, uPtr,  &Uses::useStmt,
                                     &Uses::used)) return false;
        } else if (relRefPtr->getType() == RelRefType::FOLLOWS) {
            Follows* fPtr = dynamic_cast<Follows*>(relRefPtr);
            if (!evaluateNoSynRelRef(PKBRelationship::FOLLOWS, fPtr, &Follows::follower,
                                     &Follows::followed)) return false;
        } else if (relRefPtr->getType() == RelRefType::FOLLOWST) {
            FollowsT* ftPtr = dynamic_cast<FollowsT*>(relRefPtr);
            if (!evaluateNoSynRelRef(PKBRelationship::FOLLOWST, ftPtr, &FollowsT::follower,
                                     &FollowsT::transitiveFollowed)) return false;
        } else if (relRefPtr->getType() == RelRefType::PARENT) {
            Parent* pPtr = dynamic_cast<Parent*>(relRefPtr);
            if (!evaluateNoSynRelRef(PKBRelationship::PARENT, pPtr, &Parent::parent,
                                     &Parent::child)) return false;
        } else if (relRefPtr->getType() == RelRefType::PARENTT) {
            ParentT* ptPtr = dynamic_cast<ParentT*>(relRefPtr);
            if (!evaluateNoSynRelRef(PKBRelationship::PARENTT, ptPtr, &ParentT::parent,
                                     &ParentT::transitiveChild)) return false;
        }
    }
    return true;
}