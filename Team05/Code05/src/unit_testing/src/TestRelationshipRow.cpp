#include <iostream>
#include "logging.h"
#include "PKB.h"
#include "PKB/RelationshipRow.h"
#include "catch.hpp"


TEST_CASE("ReationshipRow ==") {
    PKBField field1 = PKBField::createConcrete(VAR_NAME{ "test" });
    PKBField field2 = PKBField::createConcrete(VAR_NAME{ "test" });
    PKBField field3 = PKBField::createConcrete(VAR_NAME{ "proc_test" });
    RelationshipRow row1{ field1, field2 };
    RelationshipRow row2{ field1, field2 };
    RelationshipRow row3{ field1, field3 };

    REQUIRE(row1 == row1);
    REQUIRE(row1 == row2);

    REQUIRE_FALSE(row1 == row3);
}

TEST_CASE("ReationshipRow getFirst getSecond") {
    PKBField field1 = PKBField::createConcrete(VAR_NAME{ "test" });
    PKBField field2 = PKBField::createConcrete(VAR_NAME{ "test" });
    PKBField field3 = PKBField::createConcrete(VAR_NAME{ "proc_test" });
    RelationshipRow row1{ field1, field2 };
    RelationshipRow row2{ field1, field2 };
    
    REQUIRE(field1 == row1.getFirst());
    REQUIRE(field1 == row1.getSecond());
    REQUIRE(row1.getFirst() == row2.getFirst());
    REQUIRE(row1.getSecond() == row2.getSecond());
}
