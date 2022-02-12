#include <iostream>

#include "logging.h"
#include "PKB/ConstantTable.h"
#include "PKB/PKBField.h"
#include "catch.hpp"

#define TEST_LOG Logger() << "TestConstantTable.cpp"

TEST_CASE("ConstantTable testing") {
    ConstantTable table{};

    TEST_LOG << "Test empty ConstantTable#contains";
    REQUIRE_FALSE(table.contains(1));

    TEST_LOG << "Test duplicate ConstantTable#insert";
    table.insert(1);
    table.insert(1);
    REQUIRE(table.getSize() == 1);

    TEST_LOG << "Test non-empty ConstantTable#contains";
    REQUIRE(table.contains(1));
    REQUIRE_FALSE(table.contains(2));
}
