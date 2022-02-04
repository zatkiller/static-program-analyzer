#include "pkb/VariableTable.h"
#include "logging.h"
#include <iostream>

#include "catch.hpp"

#define TEST_LOG Logger() << "TestVariableTable.cpp "

/**
 * Ensures token structs and token type are correct.
 */
TEST_CASE("VariableTable testing") {
    VariableTable table{};
    PKBField field1{ PKBType::VARIABLE, true, Content{VAR_NAME{"a"}} };
    PKBField field2{ PKBType::VARIABLE, true, Content{VAR_NAME{"b"}} };

    TEST_LOG << "Test empty VariableTable#contains";
    REQUIRE_FALSE(table.contains(field1));

    TEST_LOG << "Test duplicate VariableTable#insert";
    table.insert(field1);
    table.insert(field1);
    REQUIRE(table.getSize() == 1);

    TEST_LOG << "Test non-empty VariableTable#contains";
    REQUIRE(table.contains(field1));
    REQUIRE_FALSE(table.contains(field2));
}