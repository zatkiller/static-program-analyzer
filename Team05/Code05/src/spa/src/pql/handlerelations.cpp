#include "handlerelations.h"

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

//return true if relations has no synonym and relationship holds.
void ClauseHandler::handleModifies(Modifies* mPtr) {
    Modifies m = *mPtr;
    EntRef modified = m.modified;
    StmtRef stmt = m.modifiesStmt;
    std::vector<std::string> synonyms;
    PKBField stmtField;
    PKBField modifiedField;
    StatementType sType = getStatementType(query.getDeclarationDesignEntity(stmt.getDeclaration()));
    if (stmt.isDeclaration()){
        synonyms.push_back(stmt.getDeclaration());
        stmtField = PKBField::createDeclaration(sType);

    } else if (stmt.isWildcard()) {
        stmtField = PKBField::createWildcard(PKBEntityType::STATEMENT);
    }

    if (modified.isDeclaration()) {
        synonyms.push_back(modified.getDeclaration());
        modifiedField = PKBField::createDeclaration(PKBEntityType::VARIABLE);
    } else if (modified.isWildcard()) {
        modifiedField = PKBField::createWildcard(PKBEntityType::VARIABLE);
    }
    //get PKBResponse, then do result join
    PKBResponse mResponse;
    mResponse = pkb->getRelationship(stmtField, modifiedField, PKBRelationship::MODIFIES);
    tableRef.join(mResponse, synonyms);

}

void ClauseHandler::handleUses(Uses* uPtr) {
    Uses u = *uPtr;
    StmtRef stmt = u.useStmt;
    EntRef used = u.used;
    std::vector<std::string> synonyms;
    PKBField stmtField;
    PKBField usedField;
    StatementType sType = getStatementType(query.getDeclarationDesignEntity(stmt.getDeclaration()));
    if (stmt.isDeclaration()){
        synonyms.push_back(stmt.getDeclaration());
        stmtField = PKBField::createDeclaration(sType);
    } else if (stmt.isWildcard()) {
        stmtField = PKBField::createWildcard(PKBEntityType::STATEMENT);
    }
    if (used.isDeclaration()) {
        synonyms.push_back(used.getDeclaration());
        usedField = PKBField::createDeclaration(PKBEntityType::VARIABLE);
    }
    //get PKBResponse
    PKBResponse uResponse;
    uResponse = pkb->getRelationship(stmtField, usedField, PKBRelationship::USES);
    // API call here
    tableRef.join(uResponse, synonyms);

}

void ClauseHandler::handleFollows(Follows* fPtr) {
    Follows f = *fPtr;
    StmtRef follower = f.follower;
    StmtRef followed = f.followed;
    std::vector<std::string> synonyms;
    PKBField followerField;
    PKBField followedField;
    StatementType followerType = getStatementType(query.getDeclarationDesignEntity(follower.getDeclaration()));
    StatementType followedType = getStatementType(query.getDeclarationDesignEntity(followed.getDeclaration()));
    if (follower.isDeclaration()) {
        synonyms.push_back(follower.getDeclaration());
        followerField = PKBField::createDeclaration(followerType);
    } else if (follower.isWildcard()) {
        followerField = PKBField::createWildcard(PKBEntityType::STATEMENT);
    }
    if (followed.isDeclaration()) {
        synonyms.push_back(followed.getDeclaration());
        followedField = PKBField::createDeclaration(followedType);
    } else if (followed.isWildcard()) {
        followedField = PKBField::createWildcard(PKBEntityType::STATEMENT);
    }

    PKBResponse fResponse = pkb->getRelationship(followerField, followedField, PKBRelationship::FOLLOWS);
    tableRef.join(fResponse, synonyms);
}

void ClauseHandler::handleFollowsT(FollowsT* ftPtr) {
    FollowsT ft = *ftPtr;
    StmtRef follower = ft.follower;
    StmtRef transitiveFollowed = ft.transitiveFollowed;
    std::vector<std::string> synonyms;
    PKBField followerField;
    PKBField transitiveFollowedField;
    StatementType followerType = getStatementType(query.getDeclarationDesignEntity(follower.getDeclaration()));
    StatementType transitiveFollowedType = getStatementType(query.getDeclarationDesignEntity(transitiveFollowed.getDeclaration()));
    if (follower.isDeclaration()) {
        synonyms.push_back(follower.getDeclaration());
        followerField = PKBField::createDeclaration(followerType);
    } else if (follower.isWildcard()) {
        followerField = PKBField::createWildcard(PKBEntityType::STATEMENT);
    }
    if (transitiveFollowed.isDeclaration()) {
        synonyms.push_back(transitiveFollowed.getDeclaration());
        transitiveFollowedField = PKBField::createDeclaration(transitiveFollowedType);
    } else if (transitiveFollowed.isWildcard()) {
        transitiveFollowedField = PKBField::createWildcard(PKBEntityType::STATEMENT);
    }

    PKBResponse ftResponse = pkb->getRelationship(followerField, transitiveFollowedField, PKBRelationship::FOLLOWST);
    tableRef.join(ftResponse, synonyms);
}

void ClauseHandler::handleParent(Parent * pPtr) {
    Parent p = *pPtr;
    StmtRef parent = p.parent;
    StmtRef child = p.child;
    std::vector<std::string> synonyms;
    PKBField parentField;
    PKBField childField;
    StatementType parentType = getStatementType(query.getDeclarationDesignEntity(parent.getDeclaration()));
    StatementType childType = getStatementType(query.getDeclarationDesignEntity(child.getDeclaration()));
    if (parent.isDeclaration()) {
        synonyms.push_back(parent.getDeclaration());
        parentField = PKBField::createDeclaration(parentType);
    } else if (parent.isWildcard()) {
        parentField = PKBField::createWildcard(PKBEntityType::STATEMENT);
    }
    if (child.isDeclaration()) {
        synonyms.push_back(child.getDeclaration());
        childField = PKBField::createDeclaration(childType);
    } else if (child.isWildcard()) {
        childField = PKBField::createWildcard(PKBEntityType::STATEMENT);
    }

    PKBResponse pResponse = pkb->getRelationship(parentField, childField, PKBRelationship::PARENT);
    tableRef.join(pResponse, synonyms);
}

void ClauseHandler::handleParentT(ParentT * ptPtr) {
    ParentT pt = *ptPtr;
    StmtRef parent = pt.parent;
    StmtRef transitiveChild = pt.transitiveChild;
    std::vector<std::string> synonyms;
    PKBField parentField;
    PKBField transitiveChildField;
    StatementType parentType = getStatementType(query.getDeclarationDesignEntity(parent.getDeclaration()));
    StatementType transitiveChildType = getStatementType(query.getDeclarationDesignEntity(transitiveChild.getDeclaration()));
    if (parent.isDeclaration()) {
        synonyms.push_back(parent.getDeclaration());
        parentField = PKBField::createDeclaration(parentType);
    } else if (parent.isWildcard()) {
        parentField = PKBField::createWildcard(PKBEntityType::STATEMENT);
    }
    if (transitiveChild.isDeclaration()) {
        synonyms.push_back(transitiveChild.getDeclaration());
        transitiveChildField = PKBField::createDeclaration(transitiveChildType);
    } else if (transitiveChild.isWildcard()) {
        transitiveChildField = PKBField::createWildcard(PKBEntityType::STATEMENT);
    }

    PKBResponse ptResponse = pkb->getRelationship(parentField, transitiveChildField, PKBRelationship::PARENT);
    tableRef.join(ptResponse, synonyms);
}

void ClauseHandler::handleSynClauses(std::vector<std::shared_ptr<RelRef>> clauses) {
    for (auto c : clauses) {
        RelRef* relRefPtr = c.get();
        RelRefType type = relRefPtr->getType();
        if (type == RelRefType::MODIFIESS) {
            handleModifies(dynamic_cast<Modifies*>(relRefPtr));
        } else if (type == RelRefType::USESS) {
            handleUses(dynamic_cast<Uses*>(relRefPtr));
        } else if (type == RelRefType::FOLLOWS) {
            handleFollows(dynamic_cast<Follows*>(relRefPtr));
        } else if (type == RelRefType::FOLLOWST) {
            handleFollowsT(dynamic_cast<FollowsT*>(relRefPtr));
        } else if (type == RelRefType::PARENT) {
            handleParent(dynamic_cast<Parent*>(relRefPtr));
        } else if (type == RelRefType::PARENTT) {
            handleParentT(dynamic_cast<ParentT*>(relRefPtr));
        }
    }
}

bool ClauseHandler::evaluateNoSynClauses(std::vector<std::shared_ptr<RelRef>> noSynClauses) {
    for (auto clause : noSynClauses) {
        RelRef* relRefPtr = clause.get();
        if (relRefPtr->getType() == RelRefType::MODIFIESS) {
            Modifies* mPtr= dynamic_cast<Modifies*>(relRefPtr);
            PKBField stmtField = PKBField::createConcrete(STMT_LO{mPtr->modifiesStmt.getLineNo()});
            PKBField modifiedField = PKBField::createConcrete(VAR_NAME{mPtr->modified.getVariableName()});
            //API call
            if (!pkb->isRelationshipPresent(stmtField, modifiedField, PKBRelationship::MODIFIES)) {
                return false;
            }
        } else if (relRefPtr->getType() == RelRefType::MODIFIESS) {
            Uses* uPtr = dynamic_cast<Uses*>(relRefPtr);
            PKBField stmtField = PKBField::createConcrete(STMT_LO{uPtr->useStmt.getLineNo()});
            PKBField usedField = PKBField::createConcrete(VAR_NAME{uPtr->used.getVariableName()});
            if (!pkb->isRelationshipPresent(stmtField, usedField, PKBRelationship::USES)) {
                return false;
            }
        } else if (relRefPtr->getType() == RelRefType::FOLLOWS) {
            Follows* fPtr = dynamic_cast<Follows*>(relRefPtr);
            PKBField followerField = PKBField::createConcrete(STMT_LO{fPtr->follower.getLineNo()});
            PKBField followedField = PKBField::createConcrete(STMT_LO{fPtr->followed.getLineNo()});
            if (!pkb->isRelationshipPresent(followerField, followedField, PKBRelationship::FOLLOWS)) {
                return false;
            }
        } else if (relRefPtr->getType() == RelRefType::FOLLOWST) {
            FollowsT* ftPtr = dynamic_cast<FollowsT*>(relRefPtr);
            PKBField followerField = PKBField::createConcrete(STMT_LO{ftPtr->follower.getLineNo()});
            PKBField transitiveFollowedField = PKBField::createConcrete(STMT_LO{ftPtr->transitiveFollowed.getLineNo()});
            if (!pkb->isRelationshipPresent(followerField, transitiveFollowedField, PKBRelationship::FOLLOWST)) {
                return false;
            }
        } else if (relRefPtr->getType() == RelRefType::PARENT) {
            Parent* pPtr = dynamic_cast<Parent*>(relRefPtr);
            PKBField parentField = PKBField::createConcrete(STMT_LO{pPtr->parent.getLineNo()});
            PKBField childField = PKBField::createConcrete(STMT_LO{pPtr->child.getLineNo()});
            if (!pkb->isRelationshipPresent(parentField, childField, PKBRelationship::PARENT)) {
                return false;
            }
        } else if (relRefPtr->getType() == RelRefType::PARENTT) {
            ParentT* ptPtr = dynamic_cast<ParentT*>(relRefPtr);
            PKBField parentField = PKBField::createConcrete(STMT_LO{ptPtr->parent.getLineNo()});
            PKBField transitiveChildField = PKBField::createConcrete(STMT_LO{ptPtr->transitiveChild.getLineNo()});
            if (!pkb->isRelationshipPresent(parentField, transitiveChildField, PKBRelationship::PARENTT)) {
                return false;
            }
        }
    }
    return true;
}