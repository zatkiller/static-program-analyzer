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

    TEST_LOG << "Test empty VariableTable#contains";
    REQUIRE_FALSE(table.contains("a"));

    TEST_LOG << "Test duplicate VariableTable#insert";
    table.insert("a");
    table.insert("a");
    REQUIRE(table.getSize() == 1);

    TEST_LOG << "Test non-empty VariableTable#contains";
    REQUIRE(table.contains("a"));
    REQUIRE_FALSE(table.contains("b"));
}