#include <iostream>

#include "logging.h"
#include "PKB.h"
#include "PKB/PKBFieldFactory.h"
#include "catch.hpp"

#define TEST_LOG Logger() << "TestPKB.cpp "

/**
 * Ensures token structs and token type are correct.
 */
TEST_CASE("PKB testing") {
    std::unique_ptr<PKB> pkb = std::unique_ptr<PKB>(new PKB());

    PKBField field1 = PKBFieldFactory::createConcreteField(PKBEntityType::PROCEDURE, Content{ PROC_NAME{"main"} });
    PKBField field2 = PKBFieldFactory::createConcreteField(PKBEntityType::VARIABLE, Content{ VAR_NAME{"a"} });
    PKBField field3 = PKBFieldFactory::createConcreteField(PKBEntityType::VARIABLE, Content{ VAR_NAME{"b"} });
    PKBField field4 = PKBFieldFactory::createConcreteField(PKBEntityType::PROCEDURE, Content{ PROC_NAME{"foo"} });
    PKBField procDeclaration = PKBFieldFactory::createProcedureDeclarationField();
    PKBField varDeclaration = PKBFieldFactory::createVariableDeclarationField();


    pkb->insertRelationship(PKBRelationship::MODIFIES, field1, field2);
    REQUIRE(pkb->isRelationshipPresent(field1, field2, PKBRelationship::MODIFIES));
    REQUIRE(pkb->isRelationshipPresent(
        PKBFieldFactory::createConcreteField(PKBEntityType::PROCEDURE, Content{ PROC_NAME{"main"} }), 
        PKBFieldFactory::createConcreteField(PKBEntityType::VARIABLE, Content{ VAR_NAME{"a"} }), 
        PKBRelationship::MODIFIES));
    REQUIRE_FALSE(pkb->isRelationshipPresent(field1, field3, PKBRelationship::MODIFIES));
    REQUIRE(*(field3.getContent<VAR_NAME>()) == VAR_NAME{ "b" });
    TEST_LOG << "Test PKB#insertRelationship MODIFIES";

    pkb->insertRelationship(PKBRelationship::MODIFIES, field1, field2);
    REQUIRE(pkb->modifiesTable->getSize() == 1);


    TEST_LOG << "Test PKB#getRelationship MODIFIES (All concrete fields)";

    // Both fields are concrete
    PKBResponse getRes1 = pkb->getRelationship(field1, field2, PKBRelationship::MODIFIES);
    PKBResponse getExpected1 = PKBResponse{ true, Response{ FieldRowResponse{{field1, field2}} } };
    REQUIRE(getRes1 == getExpected1);

    // Both fields are not concrete
    pkb->insertRelationship(PKBRelationship::MODIFIES, field1, field3);
    pkb->insertRelationship(PKBRelationship::MODIFIES, field4, field2);
    PKBResponse getRes2 = pkb->getRelationship(procDeclaration, varDeclaration, PKBRelationship::MODIFIES);
    PKBResponse getExpected2 = PKBResponse{ true, 
        Response{ FieldRowResponse{{field1, field2}, {field1, field3}, {field4, field2}} } };
    REQUIRE(getRes2 == getExpected2);


    // First field concrete, second not concrete
    PKBResponse getRes3 = pkb->getRelationship(field1, varDeclaration, PKBRelationship::MODIFIES);
    PKBResponse getExpected3 = PKBResponse{ true, Response{ FieldRowResponse{{field1, field2}, {field1, field3}} } };
    REQUIRE(getRes3 == getExpected3);

    // First field not concrete, second concrete
    PKBResponse getRes4 = pkb->getRelationship(procDeclaration, field2, PKBRelationship::MODIFIES);
    PKBResponse getExpected4 = PKBResponse{ true, Response{ FieldRowResponse{{field1, field2}, {field4, field2}} } };
    REQUIRE(getRes4 == getExpected4);

    TEST_LOG << "Test PKB#insertVariable";

    std::string varName1 = "a";
    pkb->insertVariable(varName1);
    REQUIRE(pkb->variableTable->getSize() == 1);

    pkb->insertStatement(StatementType::Assignment, 1);
    pkb->insertStatement(StatementType::While, 1);

    PKBResponse res2 = pkb->getStatements();
    auto content3 = res2.getResponse<FieldResponse>();
    REQUIRE(content3->size() == 2);

    PKBResponse res3 = pkb->getStatements(StatementType::Assignment);
    auto content4 = res3.getResponse<FieldResponse>();
    REQUIRE(content4->size() == 1);
}
