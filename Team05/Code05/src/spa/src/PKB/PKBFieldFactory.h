#pragma once

#include <stdexcept>
#include <type_traits>

#include "PKBField.h"

using std::is_same;

class PKBFieldFactory {
public:
    static PKBField createWildcardField(PKBEntityType entityType);
    static PKBField createStatementDeclarationField(StatementType statementType);
    static PKBField createConstantDeclarationField();
    static PKBField createVariableDeclarationField();
    static PKBField createProcedureDeclarationField();

    template <typename T>
    static PKBField createConcreteField(T data) {
        Content content{ data };
        PKBEntityType type{};
        
        switch (content.index()) {
        case 1:
            type = PKBEntityType::STATEMENT;
            break;
        case 2:
            type = PKBEntityType::VARIABLE;
            break;
        case 3:
            type = PKBEntityType::PROCEDURE;
            break;
        case 4:
            type = PKBEntityType::CONST;
            break;
        default:
            throw std::invalid_argument("Content of a concrete field must be provided.");
        }

        return PKBField{ PKBFieldType::CONCRETE, type, content };
    }
};
