#include "pkb/ProcedureTable.h"
#include "logging.h"
#include <iostream>

#include "catch.hpp"

#define TEST_LOG Logger() << "TestProcedureTable.cpp "

/**
 * Ensures token structs and token type are correct.
 */
TEST_CASE("ProcedureTable testing") {
    ProcedureTable table{};

    TEST_LOG << "Test empty ProcedureTable#contains";
    REQUIRE_FALSE(table.contains("main"));

    TEST_LOG << "Test duplicate ProcedureTable#insert";
    table.insert("main");
    table.insert("main");
    REQUIRE(table.getSize() == 1);

    TEST_LOG << "Test non-empty ProcedureTable#contains";
    REQUIRE(table.contains("main"));
    REQUIRE_FALSE(table.contains("foo"));
}