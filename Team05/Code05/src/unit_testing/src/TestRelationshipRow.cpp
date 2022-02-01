#include "pkb/RelationshipRow.h"
#include "logging.h"
#include <iostream>

#include "catch.hpp"

#define TEST_LOG Logger() << "TestRelationshipRow.cpp "

/**
 * 
 */
TEST_CASE("RelationshipRow operator testing") {
    // TODO: update to use PKBField
    RelationshipRow row1{ 1,2 };
    RelationshipRow row2{ 1,2 };
    RelationshipRow row3{ 1,3 };

    TEST_LOG << "Test RelationshipRow ==";
    REQUIRE(row1 == row1);
    REQUIRE(row1 == row2);

    REQUIRE_FALSE(row1 == row3);

    TEST_LOG << "Test RelationshipRow <";
    REQUIRE(row1 < row3);
    REQUIRE_FALSE(row1 < row1);
    REQUIRE_FALSE(row1 < row2);
}
