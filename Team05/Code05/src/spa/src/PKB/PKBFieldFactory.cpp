#include "PKBFieldFactory.h"

PKBField PKBFieldFactory::createConcreteField(PKBEntityType entityType, Content content) {
    // TODO(teo-jun-xiong): add check that entityType == content, e.g. if entityType is statement,
    // content should be STMT_LO
    return PKBField{ PKBFieldType::CONCRETE, entityType, content };
}

PKBField PKBFieldFactory::createWildcardField(PKBEntityType entityType) {
    return PKBField{ PKBFieldType::WILDCARD, entityType };
}

PKBField PKBFieldFactory::createStatementDeclarationField(StatementType statementType) {
    return PKBField{ PKBFieldType::DECLARATION, PKBEntityType::STATEMENT, statementType };
}

PKBField PKBFieldFactory::createConstantDeclarationField() {
    return PKBField{ PKBFieldType::DECLARATION, PKBEntityType::CONST };
}

PKBField PKBFieldFactory::createVariableDeclarationField() {
    return PKBField{ PKBFieldType::DECLARATION, PKBEntityType::VARIABLE };
}

PKBField PKBFieldFactory::createProcedureDeclarationField() {
    return PKBField{ PKBFieldType::DECLARATION, PKBEntityType::PROCEDURE };
}
