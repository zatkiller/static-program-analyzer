#include <iostream>

#include "logging.h"
#include "PKB/ConstantTable.h"
#include "PKB/PKBField.h"
#include "catch.hpp"

#define TEST_LOG Logger() << "TestConstantTable.cpp"

TEST_CASE("ConstantTable insert and contains") {
    ConstantTable table{};
    PKBField field1 = PKBField::createConcrete(1);
    PKBField field2 = PKBField::createConcrete(2);
    PKBField field3 = PKBField::createConcrete(1);
    REQUIRE_FALSE(table.contains(field1));

    table.insert(field1);
    table.insert(field3);
    REQUIRE(table.contains(PKBField::createConcrete(1)));

    REQUIRE(table.contains(field1));
    REQUIRE(table.contains(field3));
    REQUIRE_FALSE(table.contains(field2));
}

TEST_CASE("ConstantTable getSize") {
    ConstantTable table{};
    PKBField field1 = PKBField::createConcrete(1);
    REQUIRE(table.getSize() == 0);

    table.insert(field1);
    REQUIRE(table.getSize() == 1);
}

TEST_CASE("ConstantTable getAllConst") {
    ConstantTable table{};
    std::vector<CONST> expected{};
    REQUIRE(expected == table.getAllConst());

    table.insert(PKBField::createConcrete(1));
    table.insert(PKBField::createConcrete(2));
    expected = { 1, 2 };
    REQUIRE(expected == table.getAllConst());
}
