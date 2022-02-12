#include <iostream>

#include "logging.h"
#include "PKB/ConstantTable.h"
#include "PKB/PKBField.h"
#include "catch.hpp"

TEST_CASE("ConstantTable insert and contains") {
    ConstantTable table{};
    PKBField field1 = PKBField::createConcrete(CONST{ 1 });
    PKBField field2 = PKBField::createConcrete(CONST{ 2 });
    PKBField field3 = PKBField::createConcrete(CONST{ 1 });
    REQUIRE_FALSE(table.contains(CONST{ 1 }));

    table.insert(CONST{ 1 });
    table.insert(CONST{ 1 });
    REQUIRE(table.contains(CONST{ 1 }));

    REQUIRE(table.contains(CONST{ 1 }));
    REQUIRE_FALSE(table.contains(CONST{ 2 }));
}

TEST_CASE("ConstantTable getSize") {
    ConstantTable table{};
    REQUIRE(table.getSize() == 0);

    table.insert(CONST{ 1 });
    REQUIRE(table.getSize() == 1);

    table.insert(CONST{ 1 });
    REQUIRE(table.getSize() == 1);
}

TEST_CASE("ConstantTable getAllConst") {
    ConstantTable table{};
    std::vector<CONST> expected{};
    REQUIRE(expected == table.getAllConst());

    table.insert(CONST{ 1 });
    table.insert(CONST{ 2 });
    expected = { CONST{1}, CONST{2} };
    REQUIRE(expected == table.getAllConst());
}
