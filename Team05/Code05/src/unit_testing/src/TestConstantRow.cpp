#include "PKB/ConstantRow.h"
#include "logging.h"
#include <iostream>
#include "catch.hpp"

TEST_CASE("ConstantRow getConst") {
    ConstantRow row1{ CONST{1} };

    REQUIRE(row1.getConst()  == 1);
    REQUIRE(row1.getConst() == CONST{ 1 });
}

TEST_CASE("ConstantRow ==") {
    ConstantRow row1{ CONST{1} };
    ConstantRow row2{ CONST{1} };
    ConstantRow row3{ CONST{2} };

    REQUIRE(row1 == row2);
    REQUIRE(row1 == row1);
    REQUIRE_FALSE(row1 == row3);
}
