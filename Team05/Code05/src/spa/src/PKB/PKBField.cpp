#include <vector>

#include "PKBField.h"
#include "PKBDataTypes.h"

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
