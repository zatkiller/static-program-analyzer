#include "clausehandler.h"

namespace qps::evaluator {
    StatementType ClauseHandler::getStatementType(query::DesignEntity d) {
        if (d == query::DesignEntity::STMT) {
            return StatementType::Statement;
        } else if (d == query::DesignEntity::ASSIGN) {
            return StatementType::Assignment;
        } else if (d == query::DesignEntity::WHILE) {
            return StatementType::While;
        } else if (d == query::DesignEntity::IF) {
            return StatementType::If;
        } else if (d == query::DesignEntity::READ) {
            return StatementType::Read;
        } else if (d == query::DesignEntity::PRINT) {
            return StatementType::Print;
        } else if (d == query::DesignEntity::CALL) {
            return StatementType::Call;
        }
    }

    PKBField ClauseHandler::wrapStmtRef(query::StmtRef stmtRef) {
        PKBField stmtField;
        if (stmtRef.isLineNo()) {
            stmtField = PKBField::createConcrete(STMT_LO{stmtRef.getLineNo()});
        } else if (stmtRef.isWildcard()) {
            stmtField = PKBField::createWildcard(PKBEntityType::STATEMENT);
        } else if (stmtRef.isDeclaration()) {
            stmtField = PKBField::createDeclaration(
                    getStatementType(query.getDeclarationDesignEntity(stmtRef.getDeclaration())));
        }
        return stmtField;
    }

    PKBField ClauseHandler::wrapEntRef(query::EntRef entRef) {
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

    void ClauseHandler::handleSynClauses(std::vector<std::shared_ptr<query::RelRef>> clauses) {
        for (auto c : clauses) {
            query::RelRef *relRefPtr = c.get();
            query::RelRefType type = relRefPtr->getType();
            if (type == query::RelRefType::MODIFIESS) {
                evaluateRelationships(dynamic_cast<query::Modifies *>(relRefPtr), &query::Modifies::modifiesStmt,
                                      &query::Modifies::modified, PKBRelationship::MODIFIES);
            } else if (type == query::RelRefType::USESS) {
                evaluateRelationships(dynamic_cast<query::Uses *>(relRefPtr), &query::Uses::useStmt,
                                      &query::Uses::used, PKBRelationship::USES);
            } else if (type == query::RelRefType::FOLLOWS) {
                evaluateRelationships(dynamic_cast<query::Follows *>(relRefPtr), &query::Follows::follower,
                                      &query::Follows::followed, PKBRelationship::FOLLOWS);
            } else if (type == query::RelRefType::FOLLOWST) {
                evaluateRelationships(dynamic_cast<query::FollowsT *>(relRefPtr), &query::FollowsT::follower,
                                      &query::FollowsT::transitiveFollowed, PKBRelationship::FOLLOWST);
            } else if (type == query::RelRefType::PARENT) {
                evaluateRelationships(dynamic_cast<query::Parent *>(relRefPtr), &query::Parent::parent,
                                      &query::Parent::child, PKBRelationship::PARENT);
            } else if (type == query::RelRefType::PARENTT) {
                evaluateRelationships(dynamic_cast<query::ParentT *>(relRefPtr), &query::ParentT::parent,
                                      &query::ParentT::transitiveChild, PKBRelationship::PARENTT);
            }
        }
    }

    bool ClauseHandler::handleNoSynClauses(std::vector<std::shared_ptr<query::RelRef>> noSynClauses) {
        for (auto clause : noSynClauses) {
            query::RelRef *relRefPtr = clause.get();
            if (relRefPtr->getType() == query::RelRefType::MODIFIESS) {
                query::Modifies *mPtr = dynamic_cast<query::Modifies *>(relRefPtr);
                if (!evaluateNoSynRelRef(PKBRelationship::MODIFIES, mPtr, &query::Modifies::modifiesStmt,
                                         &query::Modifies::modified))
                    return false;
            } else if (relRefPtr->getType() == query::RelRefType::USESS) {
                query::Uses *uPtr = dynamic_cast<query::Uses *>(relRefPtr);
                if (!evaluateNoSynRelRef(PKBRelationship::USES, uPtr, &query::Uses::useStmt,
                                         &query::Uses::used))
                    return false;
            } else if (relRefPtr->getType() == query::RelRefType::FOLLOWS) {
                query::Follows *fPtr = dynamic_cast<query::Follows *>(relRefPtr);
                if (!evaluateNoSynRelRef(PKBRelationship::FOLLOWS, fPtr, &query::Follows::follower,
                                         &query::Follows::followed))
                    return false;
            } else if (relRefPtr->getType() == query::RelRefType::FOLLOWST) {
                query::FollowsT *ftPtr = dynamic_cast<query::FollowsT *>(relRefPtr);
                if (!evaluateNoSynRelRef(PKBRelationship::FOLLOWST, ftPtr, &query::FollowsT::follower,
                                         &query::FollowsT::transitiveFollowed))
                    return false;
            } else if (relRefPtr->getType() == query::RelRefType::PARENT) {
                query::Parent *pPtr = dynamic_cast<query::Parent *>(relRefPtr);
                if (!evaluateNoSynRelRef(PKBRelationship::PARENT, pPtr, &query::Parent::parent,
                                         &query::Parent::child))
                    return false;
            } else if (relRefPtr->getType() == query::RelRefType::PARENTT) {
                query::ParentT *ptPtr = dynamic_cast<query::ParentT *>(relRefPtr);
                if (!evaluateNoSynRelRef(PKBRelationship::PARENTT, ptPtr, &query::ParentT::parent,
                                         &query::ParentT::transitiveChild))
                    return false;
            }
        }
        return true;
    }
}  // namespace qps::evaluator
