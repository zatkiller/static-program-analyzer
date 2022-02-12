#include <iostream>
#include "logging.h"
#include "PKB/ProcedureTable.h"
#include "PKB/PKBField.h"
#include "catch.hpp"

TEST_CASE("ProcedureTable testing") {
    ProcedureTable table{};

    PKBField field1 = PKBField::createConcrete(PROC_NAME{ "main" });
    PKBField field2 = PKBField::createConcrete(PROC_NAME{ "foo" });

    REQUIRE_FALSE(table.contains(field1));

    table.insert(field1);
    table.insert(field1);
    REQUIRE(table.getSize() == 1);

    REQUIRE(table.contains(field1));
    REQUIRE_FALSE(table.contains(field2));
}

TEST_CASE("ProcedureTable insert and contains") {
    ProcedureTable table{};
    PKBField field1 = PKBField::createConcrete(PROC_NAME{ "main" });
    PKBField field2 = PKBField::createConcrete(PROC_NAME{ "foo" });
    PKBField field3 = PKBField::createConcrete(PROC_NAME{ "main" });

    REQUIRE_FALSE(table.contains(field1));

    table.insert(field1);
    table.insert(field3);
    REQUIRE(table.contains(PKBField::createConcrete(PROC_NAME{ "main" })));

    REQUIRE(table.contains(field1));
    REQUIRE(table.contains(field3));
    REQUIRE_FALSE(table.contains(field2));
}

TEST_CASE("ProcedureTable getSize") {
    ProcedureTable table{};
    PKBField field1 = PKBField::createConcrete(PROC_NAME{ "main" });
    REQUIRE(table.getSize() == 0);

    table.insert(field1);
    REQUIRE(table.getSize() == 1);

    table.insert(field1);
    REQUIRE(table.getSize() == 1);
}

TEST_CASE("ProcedureTable getAllProcs") {
    ProcedureTable table{};
    std::vector<PROC_NAME> expected{};
    REQUIRE(expected == table.getAllProcs());

    table.insert(PKBField::createConcrete(PROC_NAME{ "main" }));
    table.insert(PKBField::createConcrete(PROC_NAME{ "foo" }));
    expected = { PROC_NAME{"main"}, PROC_NAME{"foo"} };
    REQUIRE(expected == table.getAllProcs());
}
