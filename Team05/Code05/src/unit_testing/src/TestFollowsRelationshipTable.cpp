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

TEST_CASE("FollowsRelationshipTable::insert, FollowsRelationshipTable::contains") {
    auto table = std::unique_ptr<FollowsRelationshipTable>(new FollowsRelationshipTable());
    PKBField field1 = PKBField::createConcrete(STMT_LO{ 1, StatementType::Assignment });
    PKBField field2 = PKBField::createConcrete(STMT_LO{ 2, StatementType::If });
    PKBField field3 = PKBField::createConcrete(STMT_LO{ 3, StatementType::Print });
    PKBField field4 = PKBField::createConcrete(STMT_LO{ 4, StatementType::Print });
    PKBField field5 = PKBField::createConcrete(STMT_LO{ 5, StatementType::Print });

    // Empty table
    REQUIRE_FALSE(table->contains(field1, field2));

    table->insert(field1, field2);
    table->insert(field2, field3);
    table->insert(field2, field4);
    table->insert(field4, field5);

    REQUIRE(table->contains(field1, field2));
    REQUIRE_FALSE(table->contains(field2, field1));
    REQUIRE(table->contains(field2, field3));
    REQUIRE_FALSE(table->contains(field1, field3));
    REQUIRE_FALSE(table->contains(field1, field5));

    // Insert of an existing field will not replace it
    PKBField duplicate = PKBField::createConcrete(STMT_LO{ 1, StatementType::While });
    table->insert(duplicate, field2);
    REQUIRE(table->contains(field1, field2));
    REQUIRE_FALSE(table->contains(duplicate, field2));
}

TEST_CASE("FollowsRelationshipTable::retrieve") {
    auto table = std::unique_ptr<FollowsRelationshipTable>(new FollowsRelationshipTable());
    PKBField field1 = PKBField::createConcrete(STMT_LO{ 1, StatementType::Assignment });
    PKBField field2 = PKBField::createConcrete(STMT_LO{ 2, StatementType::If });
    PKBField field3 = PKBField::createConcrete(STMT_LO{ 3, StatementType::Print });

    // Case 1: Both query fields are statement declarations
    PKBField decl1 = PKBField::createDeclaration(StatementType::All);
    PKBField decl2 = PKBField::createDeclaration(StatementType::If);
    PKBField decl3 = PKBField::createDeclaration(StatementType::Assignment);
    PKBField decl4 = PKBField::createDeclaration(StatementType::Print);

    // Empty table
    REQUIRE(table->retrieve(decl1, decl1) == FieldRowResponse{});

    table->insert(field1, field2);
    table->insert(field2, field3);

    REQUIRE(table->retrieve(decl1, decl1) == FieldRowResponse{ { {field1, field2}, { field2, field3 }} });

    // Follows(s, s), ordering check
    REQUIRE(table->retrieve(decl1, decl1) == FieldRowResponse{ { {field2, field3}, { field1, field2 }} });
    REQUIRE(table->retrieve(decl2, decl1) == FieldRowResponse{ {{field2, field3}} });
    REQUIRE(table->retrieve(decl3, decl4) == FieldRowResponse{});

    // Case 2: First query field is a declaration, second is concrete
    PKBField conc1 = PKBField::createConcrete(STMT_LO{ 1, StatementType::Assignment });
    PKBField conc2 = PKBField::createConcrete(STMT_LO{ 2, StatementType::If });
    PKBField conc3 = PKBField::createConcrete(STMT_LO{ 3, StatementType::Print });
    PKBField conc4 = PKBField::createConcrete(STMT_LO{ 4, StatementType::Print });
    PKBField incompleteStatement = PKBField::createConcrete(STMT_LO{ 2 });

    REQUIRE(table->retrieve(decl1, conc2) == FieldRowResponse{ {{field1, field2}} });

    // Follows(s,2) where 2 has no type. Note: empty result is intended as type append only occurs in PKB
    REQUIRE(table->retrieve(decl1, incompleteStatement) == FieldRowResponse{});
    REQUIRE(table->retrieve(decl1, conc4) == FieldRowResponse{});
    REQUIRE(table->retrieve(decl2, conc3) == FieldRowResponse{ { {field2, field3}} });

    // Case 3: First query field is concrete, second is a declaration
    REQUIRE(table->retrieve(conc2, decl1) == FieldRowResponse{ { {field2, field3}} });
    REQUIRE(table->retrieve(conc2, decl3) == FieldRowResponse{});
    REQUIRE(table->retrieve(conc4, decl1) == FieldRowResponse{});
}

TEST_CASE("FollowsRelationshipTable::containsT") {
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
    table->insert(field7, field8);

    REQUIRE(table->containsT(field4, field8));

    REQUIRE_FALSE(table->containsT(field4, field6));
}

TEST_CASE("FollowsRelationshipTable::retrieveT case 1") {
    auto table = std::unique_ptr<FollowsRelationshipTable>(new FollowsRelationshipTable());
    PKBField field1 = PKBField::createConcrete(STMT_LO{ 1, StatementType::Assignment });
    PKBField field2 = PKBField::createConcrete(STMT_LO{ 2, StatementType::While });
    PKBField field3 = PKBField::createConcrete(STMT_LO{ 3, StatementType::Assignment });
    PKBField field4 = PKBField::createConcrete(STMT_LO{ 4, StatementType::Assignment });
    PKBField field5 = PKBField::createConcrete(STMT_LO{ 5, StatementType::While });

    table->insert(field1, field2);
    table->insert(field3, field4);
    table->insert(field2, field5);

    REQUIRE(table->retrieveT(field1, PKBField::createDeclaration(StatementType::While)) ==
        FieldRowResponse{ {field1, field2}, {field1, field5} });
    REQUIRE(table->retrieveT(field2, PKBField::createDeclaration(StatementType::While)) ==
        FieldRowResponse{ {field2, field5} });
    REQUIRE(table->retrieveT(field1, PKBField::createDeclaration(StatementType::All)) ==
        FieldRowResponse{ {field1, field2}, {field1, field5} });
    REQUIRE(table->retrieveT(field5, PKBField::createDeclaration(StatementType::While)) ==
        FieldRowResponse{ });
    REQUIRE(table->retrieveT(field1, PKBField::createDeclaration(StatementType::Assignment)) ==
        FieldRowResponse{ });
    REQUIRE(table->retrieveT(field3, PKBField::createDeclaration(StatementType::Assignment)) ==
        FieldRowResponse{ {field3, field4} });
    REQUIRE(table->retrieveT(PKBField::createDeclaration(StatementType::Assignment),
        PKBField::createDeclaration(StatementType::Assignment)) ==
        FieldRowResponse{ {field3, field4} });
    REQUIRE(table->retrieveT(PKBField::createDeclaration(StatementType::If), field5) == FieldRowResponse{ });
    REQUIRE(table->retrieveT(PKBField::createDeclaration(StatementType::While), field1) == FieldRowResponse{ });
    REQUIRE(table->retrieveT(PKBField::createDeclaration(StatementType::While), field5) ==
        FieldRowResponse{ {field2, field5 } });
    REQUIRE(table->retrieveT(PKBField::createDeclaration(StatementType::All), field5) ==
        FieldRowResponse{ {field1, field5 }, {field2, field5 } });
    REQUIRE(table->retrieveT(PKBField::createDeclaration(StatementType::Assignment), field5) ==
        FieldRowResponse{ {field1, field5 } });
    REQUIRE(table->retrieveT(PKBField::createWildcard(PKBEntityType::STATEMENT),
        PKBField::createWildcard(PKBEntityType::STATEMENT)) ==
        FieldRowResponse{ {field1, field2}, {field1, field5},
            {field2, field5}, {field3, field4} });
}

// incomplete
TEST_CASE("FollowsRelationshipTable::retrieveT case 2") {
    auto table = std::unique_ptr<FollowsRelationshipTable>(new FollowsRelationshipTable());
    PKBField field1 = PKBField::createConcrete(STMT_LO{ 1, StatementType::Assignment });
    PKBField field2 = PKBField::createConcrete(STMT_LO{ 2, StatementType::While });
    PKBField field3 = PKBField::createConcrete(STMT_LO{ 3, StatementType::Assignment });
    PKBField field4 = PKBField::createConcrete(STMT_LO{ 4, StatementType::If });
    PKBField field5 = PKBField::createConcrete(STMT_LO{ 5, StatementType::Assignment });
    PKBField field6 = PKBField::createConcrete(STMT_LO{ 6, StatementType::Assignment });
    PKBField field7 = PKBField::createConcrete(STMT_LO{ 7, StatementType::While });
    PKBField field8 = PKBField::createConcrete(STMT_LO{ 8, StatementType::Assignment });
    PKBField field9 = PKBField::createConcrete(STMT_LO{ 9, StatementType::Assignment });

    table->insert(field1, field2);
    table->insert(field2, field9);
    table->insert(field3, field4);
    table->insert(field5, field6);

    PKBField ifDecl = PKBField::createDeclaration(StatementType::If);
    PKBField whileDecl = PKBField::createDeclaration(StatementType::While);
    PKBField assnDecl = PKBField::createDeclaration(StatementType::Assignment);
    PKBField allDecl = PKBField::createDeclaration(StatementType::All);

    // Case 1: Both declarations
    // Follows*(s, s)
    REQUIRE(table->retrieveT(allDecl, allDecl) == FieldRowResponse{ {
            {field1, field2},
            {field2, field9},
            {field3, field4},
            {field5, field6},
            {field1, field9}} });

    REQUIRE(table->retrieveT(assnDecl, whileDecl) == FieldRowResponse{ { {field1, field2}} });
    REQUIRE(table->retrieveT(whileDecl, whileDecl) == FieldRowResponse{ });
    REQUIRE(table->retrieveT(assnDecl, ifDecl) == FieldRowResponse{ {field3, field4} });

    // Case 2: First field concrete
    PKBField conc1 = PKBField::createConcrete(STMT_LO{ 1, StatementType::Assignment });
    REQUIRE(table->retrieveT(conc1, allDecl) == FieldRowResponse{ { {field1, field2}, { field1, field9 }} });
    PKBField invalidConc = PKBField::createConcrete(STMT_LO{ 1, StatementType::If });

    // Follows*(1, a) where the statement type provided does not match the one stored
    REQUIRE(table->retrieveT(invalidConc, allDecl) == FieldRowResponse{ });

    PKBField conc2 = PKBField::createConcrete(STMT_LO{ 3, StatementType::Assignment });
    REQUIRE(table->retrieve(conc2, allDecl) == FieldRowResponse{ {{field3, field4}} });
    REQUIRE(table->retrieveT(conc2, whileDecl) == FieldRowResponse{ });

    // Case 3: Second field concrete
    PKBField conc3 = PKBField::createConcrete(STMT_LO{ 9, StatementType::Assignment });
    REQUIRE(table->retrieveT(allDecl, conc3) == FieldRowResponse{ {{field2, field9}, {field1, field9}} });

    PKBField conc4 = PKBField::createConcrete(STMT_LO{ 7, StatementType::While });
    REQUIRE(table->retrieveT(allDecl, conc4) == FieldRowResponse{ });

    PKBField conc5 = PKBField::createConcrete(STMT_LO{ 4, StatementType::If });
    REQUIRE(table->retrieveT(assnDecl, conc5) == FieldRowResponse{ {{field3, field4}} });

    // Case 4: Both 
    REQUIRE(table->retrieveT(conc1, conc3) == FieldRowResponse{ {{field1, field9}} });
    REQUIRE(table->retrieveT(conc5, conc4) == FieldRowResponse{ });
}
