#include <iostream>
#include <memory>

#include "logging.h"
#include "PKB/UsesRelationshipTable.h"
#include "PKB/PKBField.h"
#include "catch.hpp"

TEST_CASE("UsesRelationshipTable::getType") {
    auto table = std::unique_ptr<UsesRelationshipTable>(new UsesRelationshipTable());
    REQUIRE(table->getType() == PKBRelationship::USES);
}

TEST_CASE("UsesRelationshipTable::insert, UsesRelationshipTable::contains valid") {
    auto table = std::unique_ptr<UsesRelationshipTable>(new UsesRelationshipTable());
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


TEST_CASE("UsesRelationshipTable::insert, UsesRelationshipTable::contains invalid") {
    auto table = std::unique_ptr<UsesRelationshipTable>(new UsesRelationshipTable());
    PKBField validStmt = PKBField::createConcrete(STMT_LO{ 1, StatementType::Assignment });
    PKBField invalidStmt = PKBField::createConcrete(STMT_LO{ 2 });
    PKBField field1 = PKBField::createConcrete(VAR_NAME{ "invalid" });
    PKBField field2 = PKBField::createConcrete(VAR_NAME{ "a" });

    CHECK_THROWS(table->insert(field1, field2));

    PKBField field3 = PKBField::createConcrete(STMT_LO{ 1 });
    REQUIRE_THROWS(table->insert(field3, field2));

    PKBField field4 = PKBField::createDeclaration(PKBEntityType::VARIABLE);
    REQUIRE_THROWS(table->insert(validStmt, field4));

    PKBField field5 = PKBField::createDeclaration(PKBEntityType::PROCEDURE);
    REQUIRE_THROWS(table->insert(field5, field2));

    REQUIRE_THROWS(table->insert(invalidStmt, field2));
}


TEST_CASE("UsesRelationshipTable regression test") {
    SECTION("UsesRelationshipTable regression test #140") {
        auto table = std::unique_ptr<UsesRelationshipTable>(new UsesRelationshipTable());
        table->retrieve(PKBField::createDeclaration(StatementType::All),
            PKBField::createDeclaration(PKBEntityType::VARIABLE));

        PKBField field1 = PKBField::createConcrete(STMT_LO{ 1, StatementType::Print });
        PKBField field2 = PKBField::createConcrete(VAR_NAME("x"));
        table->insert(field1, field2);
        REQUIRE(table->contains(field1, field2));
    }

    SECTION("UsesRelationshipTable regression test #142") {
        auto table = std::unique_ptr<UsesRelationshipTable>(new UsesRelationshipTable());
        PKBField stmt1 = PKBField::createConcrete(STMT_LO{ 1, StatementType::Print });
        PKBField stmt2 = PKBField::createConcrete(STMT_LO{ 1, StatementType::Assignment });
        PKBField var1 = PKBField::createConcrete(VAR_NAME{ "x" });
        PKBField var2 = PKBField::createConcrete(VAR_NAME{ "y" });
        PKBField proc = PKBField::createConcrete(PROC_NAME{ "main" });

        table->insert(stmt1, var1);
        table->insert(stmt2, var2);
        table->insert(proc, var2);
        REQUIRE(table->retrieve(stmt1, var1) == FieldRowResponse{ {stmt1, var1} });
        REQUIRE(table->retrieve(stmt2, var2) == FieldRowResponse{ {stmt2, var2} });
        REQUIRE(table->retrieve(proc, var2) == FieldRowResponse{ {proc,var2} });
    }
}