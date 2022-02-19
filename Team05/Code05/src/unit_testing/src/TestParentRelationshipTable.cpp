#include <iostream>
#include <memory>

#include "logging.h"
#include "PKB/ParentRelationshipTable.h"
#include "PKB/PKBField.h"
#include "catch.hpp"

TEST_CASE("ParentRelationshipTable::getType") {
    auto table = std::unique_ptr<ParentRelationshipTable>(new ParentRelationshipTable());
    REQUIRE(table->getType() == PKBRelationship::PARENT);
}

TEST_CASE("ParentRelationshipTable::insert, ParentRelationshipTable::contains") {
    auto table = std::unique_ptr<ParentRelationshipTable>(new ParentRelationshipTable());
    PKBField field1 = PKBField::createConcrete(STMT_LO{ 1, StatementType::If });
    PKBField field2 = PKBField::createConcrete(STMT_LO{ 2, StatementType::While });
    PKBField field3 = PKBField::createConcrete(STMT_LO{ 3, StatementType::Assignment });
    PKBField field4 = PKBField::createConcrete(STMT_LO{ 4, StatementType::Call });
    
    // Empty table
    REQUIRE_FALSE(table->contains(field1, field2));

    table->insert(field1, field2);
    table->insert(field2, field3);
    table->insert(field2, field4);

    REQUIRE(table->contains(field2, field3));
    REQUIRE(table->contains(field1, field2));
    REQUIRE(table->contains(field2, field4));

    // Handles duplicate of existing field
    PKBField duplicate = PKBField::createConcrete(STMT_LO{ 1, StatementType::While });
    table->insert(duplicate, field2);
    REQUIRE(table->contains(field1, field2));
    REQUIRE_FALSE(table->contains(duplicate, field2));

    // No inserting of u->v where u is not a container stmt
    PKBField field5 = PKBField::createConcrete(STMT_LO{ 5, StatementType::Print });
    PKBField field6 = PKBField::createConcrete(STMT_LO{ 6, StatementType::Print });
    table->insert(field5, field6);
    REQUIRE_FALSE(table->contains(field5, field6));

    // No inserting of u->v where v comes after u
    PKBField field7 = PKBField::createConcrete(STMT_LO{ 7,  StatementType::Print });
    PKBField field8 = PKBField::createConcrete(STMT_LO{ 8, StatementType::While });
    table->insert(field8, field7);
    REQUIRE_FALSE(table->contains(field8, field7));
}

TEST_CASE("ParentRelationshipTable::retrieve") {
    auto table = std::unique_ptr<ParentRelationshipTable>(new ParentRelationshipTable());
    PKBField field1 = PKBField::createConcrete(STMT_LO{ 1, StatementType::While });
    PKBField field2 = PKBField::createConcrete(STMT_LO{ 2, StatementType::If });
    PKBField field3 = PKBField::createConcrete(STMT_LO{ 3, StatementType::Assignment });
    PKBField field4 = PKBField::createConcrete(STMT_LO{ 4, StatementType::Call });
    PKBField field5 = PKBField::createConcrete(STMT_LO{ 5, StatementType::If });
    PKBField field6 = PKBField::createConcrete(STMT_LO{ 6, StatementType::Assignment });
    PKBField field7 = PKBField::createConcrete(STMT_LO{ 7, StatementType::Assignment });
    PKBField field8 = PKBField::createConcrete(STMT_LO{ 8, StatementType::While });
    PKBField field9 = PKBField::createConcrete(STMT_LO{ 9, StatementType::Assignment});
    PKBField field10 = PKBField::createConcrete(STMT_LO{ 10, StatementType::Assignment });


    // Case 1: Both query fields are statement declarations
    PKBField decl1 = PKBField::createDeclaration(StatementType::All);
    PKBField decl2 = PKBField::createDeclaration(StatementType::If);
    PKBField decl3 = PKBField::createDeclaration(StatementType::Assignment);
    PKBField decl4 = PKBField::createDeclaration(StatementType::Call);
    PKBField decl5 = PKBField::createDeclaration(StatementType::While);

    REQUIRE(table->retrieve(decl1, decl1) == FieldRowResponse{});

    table->insert(field1, field2);
    table->insert(field2, field3);
    table->insert(field2, field4);
    table->insert(field2, field5);
    table->insert(field2, field8);
    table->insert(field2, field10);
    table->insert(field8, field9);
    table->insert(field5, field6);
    table->insert(field5, field7);

    FieldRowResponse expected1{
        {
            {field1, field2},
            {field2, field3},
            {field2, field4},
            {field2, field5},
            {field2, field8},
            {field8, field9},
            {field5, field6},
            {field5, field7},
            {field2, field10}
        }
    };
    REQUIRE(table->retrieve(decl1, decl1) == expected1);
    
    FieldRowResponse expected2{ {{field8, field9}} };
    REQUIRE(table->retrieve(decl5, decl3) == expected2);


    // Case 2: First field is a declaration, second is concrete
    PKBField conc1 = PKBField::createConcrete(STMT_LO{ 3, StatementType::Assignment });
    PKBField conc2 = PKBField::createConcrete(STMT_LO{ 4, StatementType::Call });
    PKBField conc3 = PKBField::createConcrete(STMT_LO{ 8, StatementType::While });
    PKBField conc4 = PKBField::createConcrete(STMT_LO{ 2, StatementType::If });
    PKBField incompleteStmt = PKBField::createConcrete(STMT_LO{ 5 });

    FieldRowResponse expected3{ {{field2, field3}} };
    REQUIRE(table->retrieve(decl2, conc1) == expected3);

    REQUIRE(table->retrieve(decl1, incompleteStmt) == FieldRowResponse{});
    REQUIRE(table->retrieve(decl5, conc2) == FieldRowResponse{});

    FieldRowResponse expected4{ {{field2, field8}} };
    REQUIRE(table->retrieve(decl1, conc3) == expected4);

    // Case 3: First field is concrete, second is a declaration
    FieldRowResponse expected5{ {{field2, field3}, {field2, field10}} };
    REQUIRE(table->retrieve(conc4, decl3) == expected5);
    REQUIRE(table->retrieve(conc2, decl1) == FieldRowResponse{});
    REQUIRE(table->retrieve(conc3, decl5) == FieldRowResponse{});
    REQUIRE(table->retrieve(incompleteStmt, decl3) == FieldRowResponse{});
}

TEST_CASE("ParentRelationshipTable::containsT") {
    auto table = std::unique_ptr<ParentRelationshipTable>(new ParentRelationshipTable());
    PKBField field1 = PKBField::createConcrete(STMT_LO{ 1, StatementType::While });
    PKBField field2 = PKBField::createConcrete(STMT_LO{ 2, StatementType::If });
    PKBField field3 = PKBField::createConcrete(STMT_LO{ 3, StatementType::Assignment });
    PKBField field4 = PKBField::createConcrete(STMT_LO{ 4, StatementType::Call });
    PKBField field5 = PKBField::createConcrete(STMT_LO{ 5, StatementType::If });
    PKBField field6 = PKBField::createConcrete(STMT_LO{ 6, StatementType::Assignment });
    PKBField field7 = PKBField::createConcrete(STMT_LO{ 7, StatementType::Assignment });
    PKBField field8 = PKBField::createConcrete(STMT_LO{ 8, StatementType::While });
    PKBField field9 = PKBField::createConcrete(STMT_LO{ 9, StatementType::Assignment });
    PKBField field10 = PKBField::createConcrete(STMT_LO{ 10, StatementType::Assignment });

    // Empty table
    REQUIRE_FALSE(table->containsT(field1, field1));

    table->insert(field1, field2);
    table->insert(field2, field3);
    table->insert(field2, field4);
    table->insert(field2, field5);
    table->insert(field2, field8);
    table->insert(field2, field10);
    table->insert(field8, field9);
    table->insert(field5, field6);
    table->insert(field5, field7);

    REQUIRE(table->containsT(field1, field7));
    REQUIRE(table->containsT(field2, field9));
    REQUIRE_FALSE(table->containsT(field8, field7));
    REQUIRE_FALSE(table->containsT(field7, field1));
}

TEST_CASE("ParentRelationshipTable::retrieveT") {
    auto table = std::unique_ptr<ParentRelationshipTable>(new ParentRelationshipTable());
    PKBField field1 = PKBField::createConcrete(STMT_LO{ 1, StatementType::While });
    PKBField field2 = PKBField::createConcrete(STMT_LO{ 2, StatementType::If });
    PKBField field3 = PKBField::createConcrete(STMT_LO{ 3, StatementType::Call });
    PKBField field4 = PKBField::createConcrete(STMT_LO{ 4, StatementType::Print });
    PKBField field5 = PKBField::createConcrete(STMT_LO{ 5, StatementType::Assignment });

    // Empty Table
    REQUIRE(table->retrieveT(field1, field2) == FieldRowResponse{});

    table->insert(field1, field2);
    table->insert(field2, field3);
    table->insert(field2, field4);
    table->insert(field2, field5);

    // Case 1: Both fields declarations
    PKBField decl1 = PKBField::createDeclaration(StatementType::All);
    PKBField decl2 = PKBField::createDeclaration(StatementType::While);
    PKBField decl3 = PKBField::createDeclaration(StatementType::Print);
    PKBField decl4 = PKBField::createDeclaration(StatementType::If);
    PKBField decl5 = PKBField::createDeclaration(StatementType::Call);

    PKBField conc1 = PKBField::createConcrete(STMT_LO{ 4, StatementType::Print });
    PKBField conc2 = PKBField::createConcrete(STMT_LO{ 5, StatementType::Assignment });
    PKBField conc3 = PKBField::createConcrete(STMT_LO{ 1, StatementType::While });
    PKBField conc4 = PKBField::createConcrete(STMT_LO{ 2, StatementType::If });
    PKBField invalidConc = PKBField::createConcrete(STMT_LO{ 1, StatementType::If });

    FieldRowResponse expected1{
        {
            {field1, field2},
            {field1, field3},
            {field1, field4},
            {field1, field5},
            {field2, field3},
            {field2, field4},
            {field2, field5}
        }
    };
    REQUIRE(table->retrieveT(decl1, decl1) == expected1);

    FieldRowResponse expected2{ {{field1, field4}} };
    REQUIRE(table->retrieveT(decl2, decl3) == expected2);

    REQUIRE(table->retrieveT(decl2, decl2) == FieldRowResponse{});


    // Case 2: First field declaration, second field concrete
    FieldRowResponse expected3{ {{field1, field4}, {field2, field4}} };
    REQUIRE(table->retrieveT(decl1, conc1) == expected3);

    FieldRowResponse expected4{ {{field2, field5}} };
    REQUIRE(table->retrieveT(decl4, conc2) == expected4);

    REQUIRE(table->retrieveT(decl1, conc3) == FieldRowResponse{});
    REQUIRE(table->retrieve(decl1, invalidConc) == FieldRowResponse{});


    // Case 3: First field concrete, second field declaration
    FieldRowResponse expected5{ 
        {
            {field1, field2},
            {field1, field3},
            {field1, field4},
            {field1, field5}
        } 
    };
    REQUIRE(table->retrieveT(conc3, decl1) == expected5);

    FieldRowResponse expected6{ {{field2, field3}} };
    REQUIRE(table->retrieveT(conc4, decl5) == expected6);

    REQUIRE(table->retrieveT(conc3, decl2) == FieldRowResponse{});
    REQUIRE(table->retrieveT(invalidConc, decl1) == FieldRowResponse{});
}
