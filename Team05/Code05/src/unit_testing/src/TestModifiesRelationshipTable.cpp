#include <iostream>
#include <memory>

#include "logging.h"
#include "PKB/ModifiesRelationshipTable.h"
#include "PKB/PKBField.h"
#include "catch.hpp"

#define TEST_LOG Logger() << "TestModifiesRelationshipTable.cpp "

/**
 * Ensures token structs and token type are correct.
 */
TEST_CASE("ModifiesRelationshipTable testing") {
    std::unique_ptr<ModifiesRelationshipTable> table = 
        std::unique_ptr<ModifiesRelationshipTable>(new ModifiesRelationshipTable());
    PKBField field1 = PKBField::createConcrete(PROC_NAME{"main"});
    PKBField field2 = PKBField::createConcrete(VAR_NAME{"a"});
    PKBField field3 = PKBField::createConcrete(VAR_NAME{"b"});

    TEST_LOG << "Test ModifiesRelationshipTable#getType";
    REQUIRE(table->getType() == PKBRelationship::MODIFIES);
    REQUIRE_FALSE(table->getType() == PKBRelationship::USES);

    TEST_LOG << "Test empty ModifiesRelationshipTable#contains";
    REQUIRE_FALSE(table->contains(field1, field2));

    TEST_LOG << "Test duplicate ModifiesRelationshipTable#insert";
    table->insert(field1, field2);
    table->insert(field1, field2);
    REQUIRE(table->getSize() == 1);

    TEST_LOG << "Test non-empty ModifiesRelationshipTable#contains";
    REQUIRE(table->contains(field1, field2));
    REQUIRE_FALSE(table->contains(field1, field3));
}
