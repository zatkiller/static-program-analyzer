#include <iostream>

#include "logging.h"
#include "PKB.h"
#include "PKB/PKBField.h"
#include "catch.hpp"

#define TEST_LOG Logger() << "TestPKB.cpp "

/**
 * Ensures token structs and token type are correct.
 */
TEST_CASE("PKB testing") {
    std::unique_ptr<PKB> pkb = std::unique_ptr<PKB>(new PKB());

    PKBField field1 = PKBField::createConcrete(PROC_NAME{ "main" });
    PKBField field2 = PKBField::createConcrete(VAR_NAME{ "a" });
    PKBField field3 = PKBField::createConcrete(VAR_NAME{ "b" });
    PKBField field4 = PKBField::createConcrete(PROC_NAME{ "foo" });
    PKBField procDeclaration = PKBField::createDeclaration(PKBEntityType::PROCEDURE);
    PKBField varDeclaration = PKBField::createDeclaration(PKBEntityType::VARIABLE);

    // isRSPRESENT should check both concrete
    pkb->insertRelationship(PKBRelationship::MODIFIES, field1, field2);
    REQUIRE(pkb->isRelationshipPresent(field1, field2, PKBRelationship::MODIFIES));
    REQUIRE(pkb->isRelationshipPresent(
        PKBField::createConcrete(PROC_NAME{ "main" }),
        PKBField::createConcrete(VAR_NAME{ "a" }),
        PKBRelationship::MODIFIES));
    REQUIRE_FALSE(pkb->isRelationshipPresent(field1, field3, PKBRelationship::MODIFIES));
    REQUIRE(*(field3.getContent<VAR_NAME>()) == VAR_NAME{ "b" });
    TEST_LOG << "Test PKB#insertRelationship MODIFIES";

    pkb->insertRelationship(PKBRelationship::MODIFIES, field1, field2);
    //REQUIRE(pkb->modifiesTable->getSize() == 1);


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
    //REQUIRE(pkb->variableTable->getSize() == 1);

    pkb->insertStatement(StatementType::Assignment, 1);
    pkb->insertStatement(StatementType::While, 2);

    PKBResponse res2 = pkb->getStatements();
    auto content3 = res2.getResponse<FieldResponse>();
    REQUIRE(content3->size() == 2);

    PKBResponse res3 = pkb->getStatements(StatementType::Assignment);
    auto content4 = res3.getResponse<FieldResponse>();
    REQUIRE(content4->size() == 1);
}

TEST_CASE("PKB STMT_LO empty type") {
    std::unique_ptr<PKB> pkb = std::unique_ptr<PKB>(new PKB());
    pkb->insertStatement(StatementType::Assignment, 5);
    PKBField field1 = PKBField::createConcrete(STMT_LO{ 5 });
    PKBField field2 = PKBField::createConcrete(VAR_NAME{ "a" });
    PKBField field3 = PKBField::createConcrete(STMT_LO{ 5, StatementType::Assignment });

    pkb->insertRelationship(PKBRelationship::MODIFIES, field3, field2);
    PKBResponse res = pkb->getRelationship(field1, field2, PKBRelationship::MODIFIES);
    auto content = res.getResponse<FieldRowResponse>();
    REQUIRE(content->size() == 1);

    PKBResponse res2 = pkb->getRelationship(field3, field2, PKBRelationship::MODIFIES);
    auto content2 = res2.getResponse<FieldRowResponse>();
    REQUIRE(*content == *content2); // checks that getStatementTypeOfConcreteField works
}

TEST_CASE("PKB regression test") {
    SECTION("PKB regression test #140.1") {
        std::unique_ptr<PKB> pkb = std::unique_ptr<PKB>(new PKB());
        pkb->getRelationship(
            PKBField::createDeclaration(StatementType::All),
            PKBField::createDeclaration(PKBEntityType::VARIABLE),
            PKBRelationship::USES
        );
    }
    SECTION("PKB regression test #140.2") {
        std::unique_ptr<PKB> pkb = std::unique_ptr<PKB>(new PKB());
        pkb->insertRelationship(PKBRelationship::USES,
            PKBField::createConcrete(STMT_LO{ 1, StatementType::Print }),
            PKBField::createConcrete(VAR_NAME("x"))
        );
        pkb->getRelationship(
            PKBField::createDeclaration(StatementType::All),
            PKBField::createDeclaration(PKBEntityType::VARIABLE),
            PKBRelationship::USES
        );
    }

    SECTION("PKB regression test #142.1") {
        std::unique_ptr<PKB> pkb = std::unique_ptr<PKB>(new PKB());
        pkb->insertRelationship(PKBRelationship::USES,
            PKBField::createConcrete(STMT_LO{ 1, StatementType::Print }),
            PKBField::createConcrete(VAR_NAME("x"))
        );
        pkb->insertRelationship(PKBRelationship::USES,
            PKBField::createConcrete(STMT_LO{ 2, StatementType::Assignment }),
            PKBField::createConcrete(VAR_NAME("y"))
        );
        pkb->insertRelationship(PKBRelationship::USES,
            PKBField::createConcrete(PROC_NAME{ "main" }),
            PKBField::createConcrete(VAR_NAME("y"))
        );
        REQUIRE_NOTHROW(pkb->getRelationship(
            PKBField::createDeclaration(StatementType::All),
            PKBField::createDeclaration(PKBEntityType::VARIABLE),
            PKBRelationship::USES
        ));
    }

    SECTION("PKB regression test #142.2") {
        std::unique_ptr<PKB> pkb = std::unique_ptr<PKB>(new PKB());
        pkb->insertRelationship(PKBRelationship::MODIFIES,
            PKBField::createConcrete(STMT_LO{ 1, StatementType::Print }),
            PKBField::createConcrete(VAR_NAME("x"))
        );
        pkb->insertRelationship(PKBRelationship::MODIFIES,
            PKBField::createConcrete(STMT_LO{ 2, StatementType::Assignment }),
            PKBField::createConcrete(VAR_NAME("y"))
        );
        pkb->insertRelationship(PKBRelationship::MODIFIES,
            PKBField::createConcrete(PROC_NAME{ "main" }),
            PKBField::createConcrete(VAR_NAME("y"))
        );
        REQUIRE_NOTHROW(pkb->getRelationship(
            PKBField::createDeclaration(StatementType::All),
            PKBField::createDeclaration(PKBEntityType::VARIABLE),
            PKBRelationship::MODIFIES
        ));
    }
}