#include <iostream>
#include <memory>

#include "logging.h"
#include "PKB/ModifiesRelationshipTable.h"
#include "PKB/PKBField.h"
#include "catch.hpp"

TEST_CASE("ModifiesRelationshipTable::getType") {
    auto table = std::unique_ptr<ModifiesRelationshipTable>(new ModifiesRelationshipTable());
    REQUIRE(table->getType() == PKBRelationship::MODIFIES);
}

TEST_CASE("ModifiesRelationshipTable::insert, ModifiesRelationshipTable::contains valid") {
    auto table = std::unique_ptr<ModifiesRelationshipTable>(new ModifiesRelationshipTable());
    PKBField field1 = PKBField::createConcrete(PROC_NAME{ "main" });
    PKBField field2 = PKBField::createConcrete(VAR_NAME{ "a" });
    PKBField field3 = PKBField::createConcrete(VAR_NAME{ "a" });
    PKBField field4 = PKBField::createConcrete(STMT_LO{ 1, StatementType::Assignment });
    REQUIRE_FALSE(table->contains(field1, field2));

    table->insert(field1, field2);
    table->insert(field4, field2);
    REQUIRE(table->contains(field1, field2));
    REQUIRE(table->contains(field1, field3));
    REQUIRE(table->contains(field4, field2));
}

TEST_CASE("ModifiesRelationshipTable::insert, ModifiesRelationshipTable::contains invalid") {
    auto table = std::unique_ptr<ModifiesRelationshipTable>(new ModifiesRelationshipTable());
    PKBField validStmt = PKBField::createConcrete(STMT_LO{ 1, StatementType::Assignment });
    PKBField invalidStmt = PKBField::createConcrete(STMT_LO{ 2 });
    PKBField field1 = PKBField::createConcrete(VAR_NAME{ "invalid" });
    PKBField field2 = PKBField::createConcrete(VAR_NAME{ "a" });
    REQUIRE_FALSE(table->contains(field1, field2));

    table->insert(field1, field2);
    REQUIRE_FALSE(table->contains(field1, field2));
    REQUIRE(table->getSize() == 0);

    PKBField field3 = PKBField::createConcrete(STMT_LO{ 1 });
    table->insert(field3, field2);
    REQUIRE_FALSE(table->contains(field3, field2));
    REQUIRE(table->getSize() == 0);

    PKBField field4 = PKBField::createDeclaration(PKBEntityType::VARIABLE);
    table->insert(validStmt, field4);
    REQUIRE_FALSE(table->contains(validStmt, field4));
    REQUIRE(table->getSize() == 0);

    PKBField field5 = PKBField::createDeclaration(PKBEntityType::PROCEDURE);
    table->insert(field5, field2);
    REQUIRE_FALSE(table->contains(field5, field2));
    REQUIRE(table->getSize() == 0);

    table->insert(invalidStmt, field2);
    REQUIRE_FALSE(table->contains(invalidStmt, field2));
    REQUIRE(table->getSize() == 0);
}

TEST_CASE("ModifiesRelationshipTable::getSize") {
    auto table = std::unique_ptr<ModifiesRelationshipTable>(new ModifiesRelationshipTable());
    PKBField field1 = PKBField::createConcrete(PROC_NAME{ "main" });
    PKBField field2 = PKBField::createConcrete(VAR_NAME{ "a" });
    PKBField field3 = PKBField::createConcrete(VAR_NAME{ "a" });
    REQUIRE(table->getSize() == 0);

    table->insert(field1, field2);
    REQUIRE(table->getSize() == 1);
}
