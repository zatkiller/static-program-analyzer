
#include "pkbtypematcher.h"

namespace qps::evaluator {
    using query::RelRefType;

    std::unordered_map<query::RelRefType, PKBRelationship> PKBRelationshipMap {
            {RelRefType::MODIFIESS, PKBRelationship::MODIFIES},
            {RelRefType::USESS, PKBRelationship::USES},
            {RelRefType::FOLLOWS, PKBRelationship::FOLLOWS},
            {RelRefType::FOLLOWST, PKBRelationship::FOLLOWST},
            {RelRefType::PARENT, PKBRelationship::PARENT},
            {RelRefType::PARENTT, PKBRelationship::PARENTT}
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
}
