#include "pkb/RelationshipRow.h"
#include "logging.h"
#include <iostream>

#include "catch.hpp"

#define TEST_LOG Logger() << "TestRelationshipRow.cpp "

/**
 * 
 */
TEST_CASE("RelationshipRow operator testing") {
    
    PKBField field1{ PKBType::VARIABLE, true, Content{VAR_NAME{"test"}} };
    PKBField field2{ PKBType::VARIABLE, true, Content{VAR_NAME{"test"}} };
    PKBField field3{ PKBType::PROCEDURE, true, Content{PROC_NAME{"proc_test"}} };
    
    // TODO: update to use PKBField
    RelationshipRow row1{ field1, field2 };
    RelationshipRow row2{ field1, field2 };
    RelationshipRow row3{ field1, field3 };

    TEST_LOG << "Test RelationshipRow ==";
    REQUIRE(row1 == row1);
    REQUIRE(row1 == row2);

    REQUIRE_FALSE(row1 == row3);

    TEST_LOG << "Test RelationshipRow <";
    REQUIRE(row1 < row3);
    REQUIRE_FALSE(row1 < row1);
    REQUIRE_FALSE(row1 < row2);
}
