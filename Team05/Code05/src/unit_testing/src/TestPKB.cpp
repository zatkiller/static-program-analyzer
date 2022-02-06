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
}
