#include <iostream>

#include "logging.h"
#include "PKB/FollowsRelationshipTable.h"
#include "PKB/PKBField.h"
#include "catch.hpp"


TEST_CASE("FollowsRelationshipTable getType") {
    auto table = std::unique_ptr<FollowsRelationshipTable>(new FollowsRelationshipTable());
    PKBRelationship type = table->getType();
    REQUIRE(type == PKBRelationship::FOLLOWS);
}

TEST_CASE("FollowsRelationshipTable contains and insert") {
    auto table = std::unique_ptr<FollowsRelationshipTable>(new FollowsRelationshipTable());
    PKBField field1 = PKBField::createConcrete(STMT_LO{ 1, StatementType::Assignment });
    PKBField field2 = PKBField::createConcrete(STMT_LO{ 2, StatementType::If });
    PKBField field3 = PKBField::createConcrete(STMT_LO{ 3, StatementType::Print });

    REQUIRE_FALSE(table->contains(field1, field2));

    table->insert(field1, field2);
    table->insert(field2, field3);

    REQUIRE(table->contains(field1, field2));
    REQUIRE(table->contains(field2, field3));
    REQUIRE_FALSE(table->contains(field1, field3));
}

TEST_CASE("FollowsRelationshipTable getSize") {
    auto table = std::unique_ptr<FollowsRelationshipTable>(new FollowsRelationshipTable());
    PKBField field1 = PKBField::createConcrete(STMT_LO{ 1, StatementType::Assignment });
    PKBField field2 = PKBField::createConcrete(STMT_LO{ 2, StatementType::If });
    PKBField field3 = PKBField::createConcrete(STMT_LO{ 3, StatementType::Print });

    REQUIRE(table->getSize() == 0);

    table->insert(field1, field2);
    table->insert(field2, field3);

    REQUIRE(table->getSize() == 2);

    table->insert(field1, field2);
    REQUIRE(table->getSize() == 2);
}

TEST_CASE("FollowsRelationshipTable retrieve") {
    auto table = std::unique_ptr<FollowsRelationshipTable>(new FollowsRelationshipTable());
    PKBField field1 = PKBField::createConcrete(STMT_LO{ 1, StatementType::Assignment });
    PKBField field2 = PKBField::createConcrete(STMT_LO{ 2, StatementType::If });
    PKBField field3 = PKBField::createConcrete(STMT_LO{ 3, StatementType::Print });

    // Case 1: Both query fields are statement declarations

}

TEST_CASE("FollowsRelationshipTable containsT") {
    auto table = std::unique_ptr<FollowsRelationshipTable>(new FollowsRelationshipTable());
    PKBField field1 = PKBField::createConcrete(STMT_LO{ 1, StatementType::Assignment });
    PKBField field2 = PKBField::createConcrete(STMT_LO{ 2, StatementType::Assignment });
    PKBField field3 = PKBField::createConcrete(STMT_LO{ 3, StatementType::Assignment });

    REQUIRE_FALSE(table->containsT(field1, field3));

    table->insert(field1, field2);
    table->insert(field2, field3);

    REQUIRE(table->containsT(field1, field3));

    PKBField field4 = PKBField::createConcrete(STMT_LO{ 4, StatementType::If });
    PKBField field5 = PKBField::createConcrete(STMT_LO{ 5, StatementType::Assignment });
    PKBField field6 = PKBField::createConcrete(STMT_LO{ 6, StatementType::Assignment });
    PKBField field7 = PKBField::createConcrete(STMT_LO{ 7, StatementType::Assignment }); // end if
    PKBField field8 = PKBField::createConcrete(STMT_LO{ 8, StatementType::Assignment });

    table->insert(field4, field7);
    table->insert(field5, field6);
    table->insert(field6, field7);
    table->insert(field7, field8);

    REQUIRE(table->containsT(field4, field8));

    REQUIRE_FALSE(table->containsT(field4, field6));
}

//TEST_CASE("FollowsRelationshiPTable retrieveT") {
//
//}