#include <iostream>
#include <memory>

#include "logging.h"
#include "PKB/ModifiesRelationshipTable.h"
#include "PKB/PKBField.h"
#include "catch.hpp"

TEST_CASE("ModifiesRelationshipTable getType") {
    auto table = std::unique_ptr<ModifiesRelationshipTable>(new ModifiesRelationshipTable());
    REQUIRE(table->getType() == PKBRelationship::MODIFIES);
}

TEST_CASE("ModifiesRelationshipTable valid insert and contains") {
    auto table = std::unique_ptr<ModifiesRelationshipTable>(new ModifiesRelationshipTable());
    PKBField field1 = PKBField::createConcrete(PROC_NAME{ "main" });
    PKBField field2 = PKBField::createConcrete(VAR_NAME{ "a" });
    PKBField field3 = PKBField::createConcrete(VAR_NAME{ "a" });
    REQUIRE_FALSE(table->contains(field1, field2));

    table->insert(field1, field2);
    REQUIRE(table->contains(field1, field2));
    REQUIRE(table->contains(field1, field3));
}

TEST_CASE("ModifiesRelationshipTable invalid insert and contains") {
    auto table = std::unique_ptr<ModifiesRelationshipTable>(new ModifiesRelationshipTable());
    PKBField field1 = PKBField::createConcrete(VAR_NAME{ "invalid" });
    PKBField field2 = PKBField::createConcrete(VAR_NAME{ "a" });
    REQUIRE_FALSE(table->contains(field1, field2));

    table->insert(field1, field2);
    REQUIRE_FALSE(table->contains(field1, field2));
}

TEST_CASE("ModifiesRelationshipTable getSize") {
    auto table = std::unique_ptr<ModifiesRelationshipTable>(new ModifiesRelationshipTable());
    PKBField field1 = PKBField::createConcrete(PROC_NAME{ "main" });
    PKBField field2 = PKBField::createConcrete(VAR_NAME{ "a" });
    PKBField field3 = PKBField::createConcrete(VAR_NAME{ "a" });
    REQUIRE(table->getSize() == 0);

    table->insert(field1, field2);
    REQUIRE(table->getSize() == 1);
}
