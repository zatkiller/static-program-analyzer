#include "PKB.h"
#include "logging.h"
#include <iostream>

#include "catch.hpp"

#define TEST_LOG Logger() << "TestPKB.cpp "

/**
 * Ensures token structs and token type are correct.
 */
TEST_CASE("PKB testing") {
    std::unique_ptr<PKB> pkb = std::unique_ptr<PKB>(new PKB());
    PKBField field1{ PKBType::PROCEDURE, true, Content{PROC_NAME{"main"}} };
    PKBField field2{ PKBType::VARIABLE, true, Content{VAR_NAME{"a"}} };
    PKBField field3{ PKBType::VARIABLE, true, Content{VAR_NAME{"b"}} };

    TEST_LOG << "Test PKB#insertRelationship MODIFIES";

    pkb->insertRelationship(PKBRelationship::MODIFIES, field1, field2);
    REQUIRE(pkb->modifiesTable->getSize() == 1);

    TEST_LOG << "Test PKB#getRelationship MODIFIES";

    PKBResponse res1 = pkb->getRelationship(field1, field2, PKBRelationship::MODIFIES);
    /*PKBResponse expected = PKBResponse{ true, Response{std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>{field1, field2}} };
    auto content1 = std::get<std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>>(res1.res);
    auto content2 = std::get<std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>>(expected.res);
    REQUIRE(content1.size() == content2.size());
    REQUIRE(res1.hasResult == expected.hasResult);*/

    TEST_LOG << "Test PKB#insertVariable";

    std::string varName1 = "a";
    pkb->insertVariable(varName1);
    REQUIRE(pkb->variableTable->getSize() == 1);

    pkb->insertStatement(StatementType::Assignment, 1);
    pkb->insertStatement(StatementType::While, 1);

    PKBResponse res2 = pkb->getStatements();
    auto content3 = std::get_if<std::unordered_set<PKBField, PKBFieldHash>>(&res2.res);
    REQUIRE(content3->size() == 2);

    PKBResponse res3 = pkb->getStatements(StatementType::Assignment);
    auto content4 = std::get_if<std::unordered_set<PKBField, PKBFieldHash>>(&res3.res);
    REQUIRE(content4->size() == 1);
}
