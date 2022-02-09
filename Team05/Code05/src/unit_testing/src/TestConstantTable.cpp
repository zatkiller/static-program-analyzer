#include <iostream>

#include "logging.h"
#include "PKB/ConstantTable.h"
#include "PKB/PKBFieldFactory.h"
#include "catch.hpp"

#define TEST_LOG Logger() << "TestConstantTable.cpp"

TEST_CASE("ConstantTable testing") {
    ConstantTable table{};

    PKBField field1 = PKBFieldFactory::createConcreteField(PKBEntityType::CONST, Content{ CONST{1} });
    PKBField field2 = PKBFieldFactory::createConcreteField(PKBEntityType::CONST, Content{ CONST{2} });
    PKBField field3 = PKBFieldFactory::createConcreteField(PKBEntityType::CONST, Content{ CONST{1} });

    TEST_LOG << "Test empty ConstantTable#contains";
    REQUIRE_FALSE(table.contains(field1));

    TEST_LOG << "Test duplicate ConstantTable#insert";
    table.insert(field1);
    table.insert(field3);
    REQUIRE(table.getSize() == 1);

    TEST_LOG << "Test non-empty ConstantTable#contains";
    REQUIRE(table.contains(field1));
    REQUIRE(table.contains(field3));
    REQUIRE_FALSE(table.contains(field2));
}
