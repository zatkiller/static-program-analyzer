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
}
