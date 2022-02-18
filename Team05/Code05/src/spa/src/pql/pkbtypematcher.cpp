
#include "pkbtypematcher.h"

namespace qps::evaluator {

    PKBRelationship PKBTypeMatcher::getPKBRelationship(query::RelRefType r) {
        switch (r) {
        case query::RelRefType::MODIFIESS: return
        PKBRelationship::MODIFIES;
        case query::RelRefType::USESS: return
        PKBRelationship::USES;
        case query::RelRefType::FOLLOWS: return
        PKBRelationship::FOLLOWS;
        case query::RelRefType::FOLLOWST: return
        PKBRelationship::FOLLOWST;
        case query::RelRefType::PARENT: return
        PKBRelationship::PARENT;
    //            case query::RelRefType::PARENTT: return PKBRelationship::PARENTT;
        }
    }

    StatementType PKBTypeMatcher::getStatementType(query::DesignEntity d) {
        switch (d) {
            case query::DesignEntity::STMT:
                return StatementType::All;
            case query::DesignEntity::ASSIGN:
                return StatementType::Assignment;
            case query::DesignEntity::WHILE:
                return StatementType::While;
            case query::DesignEntity::IF:
                return StatementType::If;
            case query::DesignEntity::READ:
                return StatementType::Read;
            case query::DesignEntity::PRINT:
                return StatementType::Print;
            case query::DesignEntity::CALL:
                return StatementType::Call;
        }
    }
}
