#include <tuple>
#include <vector>

#include "PKBField.h"

bool STMT_LO::operator == (const STMT_LO& other) const {
    return statementNum == other.statementNum && type == other.type && attribute == other.attribute;
}

bool STMT_LO::operator < (const STMT_LO& other) const {
    return std::tie(type, statementNum, attribute) < std::tie(other.type, other.statementNum, other.attribute);
}

bool STMT_LO::hasStatementType() const {
    return type.has_value();
}

bool STMT_LO::hasAttribute() const {
    return attribute.has_value();
}

bool VAR_NAME::operator == (const VAR_NAME& other) const {
    return name == other.name;
}

bool VAR_NAME::operator < (const VAR_NAME& other) const {
    return name < other.name;
}

bool PROC_NAME::operator == (const PROC_NAME& other) const {
    return name == other.name;
}

bool PROC_NAME::operator < (const PROC_NAME& other) const {
    return name < other.name;
}

bool PKBField::operator == (const PKBField& other) const {
    if (entityType == other.entityType && (fieldType == PKBFieldType::CONCRETE && fieldType == other.fieldType)) {
        return content == other.content;
    }
    return false;
}

PKBField PKBField::createWildcard(PKBEntityType entityType) {
    return PKBField{ PKBFieldType::WILDCARD, entityType };
}

PKBField PKBField::createDeclaration(PKBEntityType entityType) {
    if (entityType == PKBEntityType::STATEMENT) {
        throw std::invalid_argument(
            "Use the overloaded createDeclarations(StatementType) to create a statement declaration");
    }

    return PKBField{ PKBFieldType::DECLARATION, entityType };
}

PKBField PKBField::createDeclaration(StatementType statementType) {
    return PKBField{ PKBFieldType::DECLARATION, PKBEntityType::STATEMENT, statementType };
}

bool PKBField::isValidConcrete(PKBEntityType type) {
    if (fieldType != PKBFieldType::CONCRETE) return false;
    if (entityType != type) return false;
    if (content.index() == 0) return false;

    if (entityType == PKBEntityType::STATEMENT) {
        if (content.index() != 1) return false;

        auto stmt = getContent<STMT_LO>();
        if (!stmt->hasStatementType()) return false;
        if (stmt->type.value() == StatementType::All) return false;
    }

    return true;
}

size_t PKBFieldHash::operator() (const PKBField& other) const {
    PKBEntityType entityType = other.entityType;

    if (other.fieldType == PKBFieldType::CONCRETE) {
        switch (entityType) {
        case PKBEntityType::STATEMENT:
            return std::hash<int>()(std::get<STMT_LO>(other.content).statementNum);
        case PKBEntityType::VARIABLE:
            return std::hash<std::string>()(std::get<VAR_NAME>(other.content).name);
        case PKBEntityType::PROCEDURE:
            return std::hash<std::string>()(std::get<PROC_NAME>(other.content).name);
        case PKBEntityType::CONST:
            return std::hash<int>()(std::get<CONST>(other.content));
        default:
            return std::hash<PKBEntityType>()(other.entityType);
        }
    } else {
        return std::hash<PKBEntityType>()(other.entityType);
    }
}

size_t PKBFieldVectorHash::operator() (const std::vector<PKBField>& other) const {
    return std::hash<int>()(other.size());
}
