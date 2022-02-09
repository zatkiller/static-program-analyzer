#include <iostream>

#include "logging.h"
#include "PKB/ProcedureTable.h"
#include "PKB/PKBField.h"
#include "catch.hpp"

#define TEST_LOG Logger() << "TestProcedureTable.cpp "

/**
 * Ensures token structs and token type are correct.
 */
TEST_CASE("ProcedureTable testing") {
    ProcedureTable table{};

    PKBField field1 = PKBField::createConcrete(PROC_NAME{"main"});
    PKBField field2 = PKBField::createConcrete(PROC_NAME{"foo"});

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
