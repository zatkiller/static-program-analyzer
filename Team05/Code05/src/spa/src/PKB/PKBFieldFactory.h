#pragma once

#include "PKBField.h"

class PKBFieldFactory {
public:
    static PKBField createConcreteField(PKBEntityType entityType, Content content);
    static PKBField createWildcardField(PKBEntityType entityType);
    static PKBField createStatementDeclarationField(StatementType statementType);
    static PKBField createConstantDeclarationField();
    static PKBField createVariableDeclarationField();
    static PKBField createProcedureDeclarationField();
};
