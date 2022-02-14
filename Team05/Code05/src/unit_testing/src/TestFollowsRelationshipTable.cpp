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
    PKBField field4 = PKBField::createConcrete(STMT_LO{ 4, StatementType::Print });

    REQUIRE(table->getSize() == 0);

    table->insert(field1, field2);
    table->insert(field2, field3);

    REQUIRE(table->getSize() == 2);

    table->insert(field1, field2);
    REQUIRE(table->getSize() == 2);

    table->insert(field1, field4);

}

TEST_CASE("FollowsRelationshipTable retrieve") {
    auto table = std::unique_ptr<FollowsRelationshipTable>(new FollowsRelationshipTable());
    PKBField field1 = PKBField::createConcrete(STMT_LO{ 1, StatementType::Assignment });
    PKBField field2 = PKBField::createConcrete(STMT_LO{ 2, StatementType::If });
    PKBField field3 = PKBField::createConcrete(STMT_LO{ 3, StatementType::Print });

    table->insert(field1, field2);
    table->insert(field2, field3);

    // Case 1: Both query fields are statement declarations
    PKBField decl1 = PKBField::createDeclaration(StatementType::All);
    PKBField decl2 = PKBField::createDeclaration(StatementType::If);
    PKBField decl3 = PKBField::createDeclaration(StatementType::Assignment);
    PKBField decl4 = PKBField::createDeclaration(StatementType::Print);

    FieldRowResponse expected1{ {{field1, field2}, {field2, field3}} };
    REQUIRE(table->retrieve(decl1, decl1) == expected1);

    FieldRowResponse expected2{ {{field2, field3}} };
    REQUIRE(table->retrieve(decl2, decl1) == expected2);

    REQUIRE(table->retrieve(decl3, decl4).size() == 0);

    // Case 2: First query field is a declaration, second is concrete
    PKBField conc1 = PKBField::createConcrete(STMT_LO{ 1, StatementType::Assignment });
    PKBField conc2 = PKBField::createConcrete(STMT_LO{ 2, StatementType::If });
    PKBField conc3 = PKBField::createConcrete(STMT_LO{ 3, StatementType::Print });
    PKBField conc4 = PKBField::createConcrete(STMT_LO{ 4, StatementType::Print });

    FieldRowResponse expected3{ {{field1, field2}} };
    REQUIRE(table->retrieve(decl1, conc2) == expected3);
    REQUIRE(table->retrieve(decl1, conc4).size() == 0);

    FieldRowResponse expected4{ {{field2, field3}} };
    REQUIRE(table->retrieve(decl2, conc3) == expected4);

    // Case 3: First query field is concrete, second is a declaration
    FieldRowResponse expected5{ {{field2, field3}} };
    REQUIRE(table->retrieve(conc2, decl1) == expected5);
    REQUIRE(table->retrieve(conc2, decl3).size() == 0);
    REQUIRE(table->retrieve(conc4, decl1).size() == 0);
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
    PKBField field7 = PKBField::createConcrete(STMT_LO{ 7, StatementType::Assignment });
    PKBField field8 = PKBField::createConcrete(STMT_LO{ 8, StatementType::Assignment });

    table->insert(field4, field7);
    table->insert(field5, field6);
    table->insert(field6, field7);
    table->insert(field7, field8);

    REQUIRE(table->containsT(field4, field8));

    REQUIRE_FALSE(table->containsT(field4, field6));
}

TEST_CASE("FollowsRelationshiPTable retrieveT") {
    auto table = std::unique_ptr<FollowsRelationshipTable>(new FollowsRelationshipTable());
    PKBField field1 = PKBField::createConcrete(STMT_LO{ 1, StatementType::Assignment });
    PKBField field2 = PKBField::createConcrete(STMT_LO{ 2, StatementType::While });
    PKBField field3 = PKBField::createConcrete(STMT_LO{ 3, StatementType::Assignment });
    PKBField field4 = PKBField::createConcrete(STMT_LO{ 4, StatementType::Assignment });
    PKBField field5 = PKBField::createConcrete(STMT_LO{ 5, StatementType::While });

    table->insert(field1, field2);
    table->insert(field3, field4);
    table->insert(field2, field5);

    REQUIRE(table->retrieveT(field1, PKBField::createDeclaration(StatementType::While)) == FieldRowResponse{ std::vector{field1, field2}, std::vector{field1, field5} });
    REQUIRE(table->retrieveT(field2, PKBField::createDeclaration(StatementType::While)) == FieldRowResponse{ std::vector{field2, field5} });
    REQUIRE(table->retrieveT(field1, PKBField::createDeclaration(StatementType::All)) == FieldRowResponse{ std::vector{field1, field2}, std::vector{field1, field5} });
    REQUIRE(table->retrieveT(field5, PKBField::createDeclaration(StatementType::While)) == FieldRowResponse{  });

    REQUIRE(table->retrieveT(field1, PKBField::createDeclaration(StatementType::Assignment)) == FieldRowResponse{});
    REQUIRE(table->retrieveT(field3, PKBField::createDeclaration(StatementType::Assignment)) == FieldRowResponse{ std::vector{field3, field4} });
    REQUIRE(table->retrieveT(PKBField::createDeclaration(StatementType::Assignment), PKBField::createDeclaration(StatementType::Assignment)) == FieldRowResponse{ std::vector{field3, field4} });

    REQUIRE(table->retrieveT(PKBField::createDeclaration(StatementType::If), field5) == FieldRowResponse{  });
    REQUIRE(table->retrieveT(PKBField::createDeclaration(StatementType::While), field1) == FieldRowResponse{  });
    REQUIRE(table->retrieveT(PKBField::createDeclaration(StatementType::While), field5) == FieldRowResponse{ std::vector{field2, field5 } });
    REQUIRE(table->retrieveT(PKBField::createDeclaration(StatementType::All), field5) == FieldRowResponse{ std::vector{field1, field5 }, std::vector{field2, field5 } });
    REQUIRE(table->retrieveT(PKBField::createDeclaration(StatementType::Assignment), field5) == FieldRowResponse{ std::vector{field1, field5 } });
    REQUIRE(table->retrieveT(PKBField::createWildcard(PKBEntityType::STATEMENT), PKBField::createWildcard(PKBEntityType::STATEMENT)) == FieldRowResponse{ std::vector{field1, field2}, std::vector{field1, field5}, std::vector{field2, field5}, std::vector{field3, field4} });
}
