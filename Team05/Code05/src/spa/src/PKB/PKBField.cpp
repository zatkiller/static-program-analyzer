#include <vector>

#include "PKBField.h"
#include "PKBDataTypes.h"

bool PKBField::operator == (const PKBField& other) const {
    if (entityType == other.entityType && (fieldType == PKBFieldType::CONCRETE && fieldType == other.fieldType)) {
        return content == other.content;
    }
    return false;
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
