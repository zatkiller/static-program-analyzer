#include "DesignExtractor/PKBStrategy.h"


void ActualPKBStrategy::insertStatement(STMT_LO stmt) {
    pkb->insertStatement(stmt.type, stmt.statementNum);
};

void ActualPKBStrategy::insertVariable(std::string name) {
    pkb->insertVariable(name);
};

void ActualPKBStrategy::insertRelationship(PKBRelationship type, Content arg1, Content arg2) {
    pkb->insertRelationship(
        type,
        PKBField::createConcrete(arg1),
        PKBField::createConcrete(arg2)
    );
};