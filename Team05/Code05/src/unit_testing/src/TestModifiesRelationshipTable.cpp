#include "pkb/ModifiesRelationshipTable.h"
#include "logging.h"
#include <iostream>

#include "catch.hpp"

#define TEST_LOG Logger() << "TestModifiesRelationshipTable.cpp "

/**
 * Ensures token structs and token type are correct.
 */
TEST_CASE("ModifiesRelationshipTable testing") {
    ModifiesRelationshipTable table{};
    
    TEST_LOG << "Test ModifiesRelationshipTable#getType";
    // TODO: change integers to PKBRelationshipType
    REQUIRE(table.getType() == 1);
    REQUIRE_FALSE(table.getType() == 2);

    TEST_LOG << "Test empty ModifiesRelationshipTable#contains";
    REQUIRE_FALSE(table.contains(1, 2));

    TEST_LOG << "Test duplicate ModifiesRelationshipTable#insert";
    table.insert(1, 2);
    table.insert(1, 2);
    REQUIRE(table.getSize() == 1);

    TEST_LOG << "Test non-empty ModifiesRelationshipTable#contains";
    REQUIRE(table.contains(1, 2));
    REQUIRE_FALSE(table.contains(1, 3));
}