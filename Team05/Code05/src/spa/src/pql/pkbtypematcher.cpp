
#include "pkbtypematcher.h"

namespace qps::evaluator {
    using query::RelRefType;
    PKBRelationship PKBTypeMatcher::getPKBRelationship(query::RelRefType r) {
        switch (r) {
            case RelRefType::MODIFIESS:
                return PKBRelationship::MODIFIES;
            case RelRefType::USESS:
                return PKBRelationship::USES;
            case RelRefType::FOLLOWS:
                return PKBRelationship::FOLLOWS;
            case RelRefType::FOLLOWST:
                return PKBRelationship::FOLLOWST;
            case RelRefType::PARENT:
                return PKBRelationship::PARENT;
            case RelRefType::PARENTT:
                return PKBRelationship::PARENTT;
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
}  // namespace qps::evaluator
