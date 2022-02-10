#include <iostream>

#include "logging.h"
#include "PKB.h"
#include "PKB/RelationshipRow.h"
#include "catch.hpp"

#define TEST_LOG Logger() << "TestRelationshipRow.cpp "

/**
 * 
 */
TEST_CASE("RelationshipRow operator testing") {  
    PKBField field1 = PKBField::createConcrete(VAR_NAME{"test"});
    PKBField field2 = PKBField::createConcrete(VAR_NAME{"test"});
    PKBField field3 = PKBField::createConcrete(VAR_NAME{"proc_test"});


    RelationshipRow row1{ field1, field2 };
    RelationshipRow row2{ field1, field2 };
    RelationshipRow row3{ field1, field3 };

    TEST_LOG << "Test RelationshipRow ==";
    REQUIRE(row1 == row1);
    REQUIRE(row1 == row2);

    REQUIRE_FALSE(row1 == row3);
}
