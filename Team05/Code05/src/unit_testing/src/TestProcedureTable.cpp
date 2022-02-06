#include "PKB/ProcedureTable.h"
#include "logging.h"
#include <iostream>

#include "catch.hpp"

#define TEST_LOG Logger() << "TestProcedureTable.cpp "

/**
 * Ensures token structs and token type are correct.
 */
TEST_CASE("ProcedureTable testing") {
    ProcedureTable table{};
    PKBField field1{ PKBType::PROCEDURE, true, Content{PROC_NAME{"main"}} };
    PKBField field2{ PKBType::PROCEDURE, true, Content{PROC_NAME{"foo"}} };

    TEST_LOG << "Test empty ProcedureTable#contains";
    REQUIRE_FALSE(table.contains(field1));

    TEST_LOG << "Test duplicate ProcedureTable#insert";
    table.insert(field1);
    table.insert(field1);
    REQUIRE(table.getSize() == 1);

    TEST_LOG << "Test non-empty ProcedureTable#contains";
    REQUIRE(table.contains(field1));
    REQUIRE_FALSE(table.contains(field2));
}