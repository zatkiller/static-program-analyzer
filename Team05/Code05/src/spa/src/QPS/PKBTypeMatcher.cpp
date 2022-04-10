
#include "PKBTypeMatcher.h"

namespace qps::evaluator {
    using query::RelRefType;

    std::unordered_map<query::RelRefType, PKBRelationship> PKBRelationshipMap {
            {RelRefType::MODIFIESS, PKBRelationship::MODIFIES},
            {RelRefType::MODIFIESP, PKBRelationship::MODIFIES},
            {RelRefType::USESS, PKBRelationship::USES},
            {RelRefType::USESP, PKBRelationship::USES},
            {RelRefType::FOLLOWS, PKBRelationship::FOLLOWS},
            {RelRefType::FOLLOWST, PKBRelationship::FOLLOWST},
            {RelRefType::PARENT, PKBRelationship::PARENT},
            {RelRefType::PARENTT, PKBRelationship::PARENTT},
            {RelRefType::CALLS, PKBRelationship::CALLS},
            {RelRefType::CALLST, PKBRelationship::CALLST},
            {RelRefType::NEXT, PKBRelationship::NEXT},
            {RelRefType::NEXTT, PKBRelationship::NEXTT},
            {RelRefType::AFFECTS, PKBRelationship::AFFECTS},
            {RelRefType::AFFECTST, PKBRelationship::AFFECTST}
    };

    std::unordered_map<query::DesignEntity, StatementType> statementTypeMap {
            {query::DesignEntity::STMT, StatementType::All},
            {query::DesignEntity::ASSIGN, StatementType::Assignment},
            {query::DesignEntity::WHILE, StatementType::While},
            {query::DesignEntity::IF, StatementType::If},
            {query::DesignEntity::READ, StatementType::Read},
            {query::DesignEntity::PRINT, StatementType::Print},
            {query::DesignEntity::CALL, StatementType::Call}
    };

    PKBRelationship PKBTypeMatcher::getPKBRelationship(query::RelRefType r) {
        return PKBRelationshipMap.find(r)->second;
    }

    StatementType PKBTypeMatcher::getStatementType(query::DesignEntity d) {
        return statementTypeMap.find(d)->second;
    }
}  // namespace qps::evaluator
