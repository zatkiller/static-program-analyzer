#include <iostream>
#include "logging.h"
#include "PKB/ProcedureTable.h"
#include "PKB/PKBField.h"
#include "catch.hpp"

TEST_CASE("ProcedureTable insert and contains") {
    ProcedureTable table{};
    REQUIRE_FALSE(table.contains("main"));

    table.insert("main");
    table.insert("main");
    REQUIRE(table.contains("main"));

    REQUIRE(table.contains("main"));
    REQUIRE_FALSE(table.contains("foo"));
}

TEST_CASE("ProcedureTable getSize") {
    ProcedureTable table{};
    REQUIRE(table.getSize() == 0);

    table.insert("main");
    REQUIRE(table.getSize() == 1);

    table.insert("main");
    REQUIRE(table.getSize() == 1);
}

TEST_CASE("ProcedureTable getAllProcs") {
    ProcedureTable table{};
    std::vector<PROC_NAME> expected{};
    REQUIRE(expected == table.getAllProcs());

    table.insert("main");
    table.insert("foo");
    expected = { PROC_NAME{"foo"}, PROC_NAME{"main"} };
    REQUIRE(expected == table.getAllProcs());
}
