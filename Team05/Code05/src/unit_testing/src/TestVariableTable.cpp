#include "PKB/VariableTable.h"
#include "logging.h"
#include <iostream>

#include "catch.hpp"

#define TEST_LOG Logger() << "TestVariableTable.cpp "

/**
 * Ensures token structs and token type are correct.
 */
TEST_CASE("VariableTable testing") {
    VariableTable table{};
    std::string s1{ "a" };
    std::string s2{ "b" };

    TEST_LOG << "Test empty VariableTable#contains";
    REQUIRE_FALSE(table.contains(s1));

    TEST_LOG << "Test duplicate VariableTable#insert";
    table.insert(s1);
    table.insert(s1);
    REQUIRE(table.getSize() == 1);

    TEST_LOG << "Test non-empty VariableTable#contains";
    REQUIRE(table.contains(s1));
    REQUIRE_FALSE(table.contains(s2));
}
