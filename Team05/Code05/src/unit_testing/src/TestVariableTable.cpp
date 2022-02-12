#include "PKB/VariableTable.h"
#include "logging.h"
#include <iostream>
#include "catch.hpp"

TEST_CASE("VariableTable insert and contains") {
    VariableTable table{};
    std::string s1{ "a" };
    std::string s2{ "b" };
    REQUIRE_FALSE(table.contains(s1));

    table.insert(s1);
    table.insert(s1);
    REQUIRE(table.getSize() == 1);
    REQUIRE(table.contains(s1));
    REQUIRE_FALSE(table.contains(s2));
}

TEST_CASE("VariableTable getSize") {
    VariableTable table{};
    REQUIRE(table.getSize() == 0);

    table.insert("a");
    REQUIRE(table.getSize() == 1);

    table.insert("a");
    REQUIRE(table.getSize() == 1);
}

TEST_CASE("VariableTable getAllVars") {
    VariableTable table{};
    std::vector<VAR_NAME> expected{};
    REQUIRE(expected == table.getAllVars());

    table.insert("a");
    table.insert("b");
    expected = { VAR_NAME{"a"}, VAR_NAME{"b"} };
    REQUIRE(expected == table.getAllVars());
}
