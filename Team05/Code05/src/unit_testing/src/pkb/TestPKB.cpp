#include <iostream>

#include "logging.h"
#include "PKB.h"
#include "PKB/PKBField.h"
#include "Parser/Parser.h"
#include "catch.hpp"

#define TEST_LOG Logger() << "TestPKB.cpp "

TEST_CASE("PKB sample source program test - Iteration 1") {
    std::unique_ptr<PKB> pkb = std::unique_ptr<PKB>(new PKB());

    /*
    procedure Example {
     1    x = 2;
     2    z = 3;
     3    i = 5;
     4    read monke;
     5    while (i!=0) {
     6      x = x - 1;
     7      if (x==1) then {
     8          z = x + 1; }
            else {
     9          y = z + x; }
    10      z = z + x + i;
    11      i = i - 1; }
    12    print monke;
    13    call abc;
    }

    procedure abc {
    14    call def;
    }

    procedure def {
    15    w = 1;
    }
    */

    PKBField const1 = PKBField::createConcrete(CONST{ 1 });
    PKBField const2 = PKBField::createConcrete(CONST{ 2 });
    PKBField const3 = PKBField::createConcrete(CONST{ 3 });
    PKBField const5 = PKBField::createConcrete(CONST{ 5 });
    PKBField stmt1 = PKBField::createConcrete(STMT_LO{ 1, StatementType::Assignment });
    PKBField stmt2 = PKBField::createConcrete(STMT_LO{ 2, StatementType::Assignment });
    PKBField stmt3 = PKBField::createConcrete(STMT_LO{ 3, StatementType::Assignment });
    PKBField stmt4 = PKBField::createConcrete(STMT_LO{ 4, StatementType::Read, "monke" });
    PKBField stmt5 = PKBField::createConcrete(STMT_LO{ 5, StatementType::While });
    PKBField stmt6 = PKBField::createConcrete(STMT_LO{ 6, StatementType::Assignment });
    PKBField stmt7 = PKBField::createConcrete(STMT_LO{ 7, StatementType::If });
    PKBField stmt8 = PKBField::createConcrete(STMT_LO{ 8, StatementType::Assignment });
    PKBField stmt9 = PKBField::createConcrete(STMT_LO{ 9, StatementType::Assignment });
    PKBField stmt10 = PKBField::createConcrete(STMT_LO{ 10, StatementType::Assignment });
    PKBField stmt11 = PKBField::createConcrete(STMT_LO{ 11, StatementType::Assignment });
    PKBField stmt12 = PKBField::createConcrete(STMT_LO{ 12, StatementType::Print, "monke" });
    PKBField stmt13 = PKBField::createConcrete(STMT_LO{ 13, StatementType::Call, "abc" });
    PKBField stmt14 = PKBField::createConcrete(STMT_LO{ 14, StatementType::Call, "def" });
    PKBField stmt15 = PKBField::createConcrete(STMT_LO{ 15, StatementType::Assignment });
    PKBField Example = PKBField::createConcrete(PROC_NAME{ "Example" });
    PKBField abc = PKBField::createConcrete(PROC_NAME{ "abc" });
    PKBField def = PKBField::createConcrete(PROC_NAME{ "def" });
    PKBField x = PKBField::createConcrete(VAR_NAME{ "x" });
    PKBField y = PKBField::createConcrete(VAR_NAME{ "y" });
    PKBField z = PKBField::createConcrete(VAR_NAME{ "z" });
    PKBField i = PKBField::createConcrete(VAR_NAME{ "i" });
    PKBField w = PKBField::createConcrete(VAR_NAME{ "w" });
    PKBField monke = PKBField::createConcrete(VAR_NAME{ "monke" });

    SECTION("PKB::validate with empty tables") {
        pkb->insertRelationship(PKBRelationship::FOLLOWS, stmt1, stmt2);
        pkb->insertRelationship(PKBRelationship::MODIFIES, stmt1, x);
        pkb->insertRelationship(PKBRelationship::USES, Example, x);
        pkb->insertRelationship(PKBRelationship::PARENT, stmt5, stmt6);
        pkb->insertRelationship(PKBRelationship::CALLS, Example, abc);
        REQUIRE_FALSE(pkb->isRelationshipPresent(stmt1, stmt2, PKBRelationship::FOLLOWS));
        REQUIRE_FALSE(pkb->isRelationshipPresent(stmt1, x, PKBRelationship::MODIFIES));
        REQUIRE_FALSE(pkb->isRelationshipPresent(Example, x, PKBRelationship::USES));
        REQUIRE_FALSE(pkb->isRelationshipPresent(stmt5, stmt6, PKBRelationship::PARENT));
        REQUIRE_FALSE(pkb->isRelationshipPresent(Example, abc, PKBRelationship::CALLS));
    }

    pkb->insertEntity(PROC_NAME{ "Example" });
    pkb->insertEntity(PROC_NAME{ "abc" });
    pkb->insertEntity(PROC_NAME{ "def" });
    pkb->insertEntity(VAR_NAME{ "x" });
    pkb->insertEntity(VAR_NAME{ "z" });
    pkb->insertEntity(VAR_NAME{ "i" });
    pkb->insertEntity(VAR_NAME{ "y" });
    pkb->insertEntity(VAR_NAME{ "w" });
    pkb->insertEntity(VAR_NAME{ "monke" });
    pkb->insertEntity(2);
    pkb->insertEntity(3);
    pkb->insertEntity(5);
    pkb->insertEntity(1);

    SECTION("PKB::validate with partially empty tables") {
        pkb->insertRelationship(PKBRelationship::FOLLOWS, stmt1, stmt2);
        pkb->insertRelationship(PKBRelationship::MODIFIES, stmt1, x);
        pkb->insertRelationship(PKBRelationship::USES, Example, x);
        pkb->insertRelationship(PKBRelationship::PARENT, stmt5, stmt6);
        pkb->insertRelationship(PKBRelationship::CALLS, Example, abc);

        // relationships involving statements will not be inserted because the statements are not in StatementTable
        REQUIRE_FALSE(pkb->isRelationshipPresent(stmt1, stmt2, PKBRelationship::FOLLOWS));
        REQUIRE_FALSE(pkb->isRelationshipPresent(stmt1, x, PKBRelationship::MODIFIES));
        REQUIRE(pkb->isRelationshipPresent(Example, x, PKBRelationship::USES));
        REQUIRE_FALSE(pkb->isRelationshipPresent(stmt5, stmt6, PKBRelationship::PARENT));
        REQUIRE(pkb->isRelationshipPresent(Example, abc, PKBRelationship::CALLS));
    }

    pkb->insertEntity(STMT_LO{ 1, StatementType::Assignment });
    pkb->insertEntity(STMT_LO{ 2, StatementType::Assignment });
    pkb->insertEntity(STMT_LO{ 3, StatementType::Assignment });
    pkb->insertEntity(STMT_LO{ 4, StatementType::Read, "monke" });
    pkb->insertEntity(STMT_LO{ 5, StatementType::While });
    pkb->insertEntity(STMT_LO{ 6, StatementType::Assignment });
    pkb->insertEntity(STMT_LO{ 7, StatementType::If });
    pkb->insertEntity(STMT_LO{ 8, StatementType::Assignment });
    pkb->insertEntity(STMT_LO{ 9, StatementType::Assignment });
    pkb->insertEntity(STMT_LO{ 10, StatementType::Assignment });
    pkb->insertEntity(STMT_LO{ 11, StatementType::Assignment });


    pkb->insertEntity(STMT_LO{ 12, StatementType::Print, "monke" });
    pkb->insertEntity(STMT_LO{ 13, StatementType::Call, "abc" });
    pkb->insertEntity(STMT_LO{ 14, StatementType::Call, "def" });
    pkb->insertEntity(STMT_LO{ 15, StatementType::Assignment, });

    pkb->insertRelationship(PKBRelationship::NEXT, stmt1, stmt2);
    pkb->insertRelationship(PKBRelationship::NEXT, stmt2, stmt3);
    pkb->insertRelationship(PKBRelationship::NEXT, stmt3, stmt4);
    pkb->insertRelationship(PKBRelationship::NEXT, stmt4, stmt5);
    pkb->insertRelationship(PKBRelationship::NEXT, stmt5, stmt6);
    pkb->insertRelationship(PKBRelationship::NEXT, stmt5, stmt12);
    pkb->insertRelationship(PKBRelationship::NEXT, stmt6, stmt7);
    pkb->insertRelationship(PKBRelationship::NEXT, stmt7, stmt8);
    pkb->insertRelationship(PKBRelationship::NEXT, stmt7, stmt9);
    pkb->insertRelationship(PKBRelationship::NEXT, stmt8, stmt10);
    pkb->insertRelationship(PKBRelationship::NEXT, stmt9, stmt10);
    pkb->insertRelationship(PKBRelationship::NEXT, stmt10, stmt11);
    pkb->insertRelationship(PKBRelationship::NEXT, stmt11, stmt5);
    pkb->insertRelationship(PKBRelationship::NEXT, stmt11, stmt12);
    pkb->insertRelationship(PKBRelationship::NEXT, stmt12, stmt13);

    pkb->insertRelationship(PKBRelationship::FOLLOWS, stmt1, stmt2);
    pkb->insertRelationship(PKBRelationship::FOLLOWS, stmt2, stmt3);
    pkb->insertRelationship(PKBRelationship::FOLLOWS, stmt3, stmt4);
    pkb->insertRelationship(PKBRelationship::FOLLOWS, stmt4, stmt5);
    pkb->insertRelationship(PKBRelationship::FOLLOWS, stmt5, stmt12);
    pkb->insertRelationship(PKBRelationship::FOLLOWS, stmt6, stmt7);
    pkb->insertRelationship(PKBRelationship::FOLLOWS, stmt7, stmt10);
    pkb->insertRelationship(PKBRelationship::FOLLOWS, stmt10, stmt11);
    pkb->insertRelationship(PKBRelationship::FOLLOWS, stmt12, stmt13);

    pkb->insertRelationship(PKBRelationship::MODIFIES, stmt1, x);
    pkb->insertRelationship(PKBRelationship::MODIFIES, stmt2, z);
    pkb->insertRelationship(PKBRelationship::MODIFIES, stmt3, i);
    pkb->insertRelationship(PKBRelationship::MODIFIES, stmt4, monke);
    pkb->insertRelationship(PKBRelationship::MODIFIES, stmt5, x);
    pkb->insertRelationship(PKBRelationship::MODIFIES, stmt5, z);
    pkb->insertRelationship(PKBRelationship::MODIFIES, stmt5, y);
    pkb->insertRelationship(PKBRelationship::MODIFIES, stmt5, i);
    pkb->insertRelationship(PKBRelationship::MODIFIES, stmt6, x);
    pkb->insertRelationship(PKBRelationship::MODIFIES, stmt7, z);
    pkb->insertRelationship(PKBRelationship::MODIFIES, stmt7, y);
    pkb->insertRelationship(PKBRelationship::MODIFIES, stmt8, z);
    pkb->insertRelationship(PKBRelationship::MODIFIES, stmt9, y);
    pkb->insertRelationship(PKBRelationship::MODIFIES, stmt10, z);
    pkb->insertRelationship(PKBRelationship::MODIFIES, stmt11, i);
    pkb->insertRelationship(PKBRelationship::MODIFIES, Example, x);
    pkb->insertRelationship(PKBRelationship::MODIFIES, Example, y);
    pkb->insertRelationship(PKBRelationship::MODIFIES, Example, z);
    pkb->insertRelationship(PKBRelationship::MODIFIES, Example, i);
    pkb->insertRelationship(PKBRelationship::MODIFIES, Example, monke);
    pkb->insertRelationship(PKBRelationship::MODIFIES, def, w);
    pkb->insertRelationship(PKBRelationship::MODIFIES, abc, w);
    pkb->insertRelationship(PKBRelationship::MODIFIES, Example, w);

    pkb->insertRelationship(PKBRelationship::USES, Example, x);
    pkb->insertRelationship(PKBRelationship::USES, Example, z);
    pkb->insertRelationship(PKBRelationship::USES, Example, i);
    pkb->insertRelationship(PKBRelationship::USES, Example, monke);
    pkb->insertRelationship(PKBRelationship::USES, stmt5, i);
    pkb->insertRelationship(PKBRelationship::USES, stmt5, z);
    pkb->insertRelationship(PKBRelationship::USES, stmt5, x);
    pkb->insertRelationship(PKBRelationship::USES, stmt6, x);
    pkb->insertRelationship(PKBRelationship::USES, stmt7, x);
    pkb->insertRelationship(PKBRelationship::USES, stmt7, z);
    pkb->insertRelationship(PKBRelationship::USES, stmt8, x);
    pkb->insertRelationship(PKBRelationship::USES, stmt9, x);
    pkb->insertRelationship(PKBRelationship::USES, stmt9, z);
    pkb->insertRelationship(PKBRelationship::USES, stmt10, x);
    pkb->insertRelationship(PKBRelationship::USES, stmt10, z);
    pkb->insertRelationship(PKBRelationship::USES, stmt10, i);
    pkb->insertRelationship(PKBRelationship::USES, stmt11, i);
    pkb->insertRelationship(PKBRelationship::USES, stmt12, monke);

    pkb->insertRelationship(PKBRelationship::PARENT, stmt5, stmt6);
    pkb->insertRelationship(PKBRelationship::PARENT, stmt5, stmt7);
    pkb->insertRelationship(PKBRelationship::PARENT, stmt7, stmt8);
    pkb->insertRelationship(PKBRelationship::PARENT, stmt7, stmt9);
    pkb->insertRelationship(PKBRelationship::PARENT, stmt5, stmt10);
    pkb->insertRelationship(PKBRelationship::PARENT, stmt5, stmt11);

    pkb->insertRelationship(PKBRelationship::CALLS, Example, abc);
    pkb->insertRelationship(PKBRelationship::CALLS, abc, def);

    SECTION("PKB::getConstants, PKB::getProcedures, PKB::getVariables, PKB::getStatements") {
        REQUIRE(pkb->getConstants() == PKBResponse{ true, FieldResponse{ const1, const2, const3, const5 } });
        REQUIRE(pkb->getProcedures() == PKBResponse{ true, FieldResponse{ Example, abc, def } });
        REQUIRE(pkb->getVariables() == PKBResponse{ true, FieldResponse{ x, y, z, i, monke, w } });
        REQUIRE(pkb->getStatements() == PKBResponse{ true, FieldResponse{ stmt1, stmt2, stmt3, stmt4, stmt5, stmt6,
            stmt7, stmt8, stmt9, stmt10, stmt11, stmt12, stmt13, stmt14, stmt15} });
    }

    SECTION("PKB::isRelationshipPresent") {
        REQUIRE(pkb->isRelationshipPresent(stmt1, stmt2, PKBRelationship::FOLLOWS));
        REQUIRE(pkb->isRelationshipPresent(stmt1, stmt3, PKBRelationship::FOLLOWST));

        // Incomplete concrete statement fields
        REQUIRE(pkb->isRelationshipPresent(PKBField::createConcrete(STMT_LO{ 1 }), stmt2,
            PKBRelationship::FOLLOWS));
        REQUIRE(pkb->isRelationshipPresent(PKBField::createConcrete(STMT_LO{ 1 }), stmt3,
            PKBRelationship::FOLLOWST));
        REQUIRE(pkb->isRelationshipPresent(PKBField::createConcrete(STMT_LO{ 1 }),
            PKBField::createConcrete(STMT_LO{ 2 }), PKBRelationship::FOLLOWS));

        // Concrete statement will All type will be converted by PKB::appendStatementInformation
        REQUIRE(pkb->isRelationshipPresent(PKBField::createConcrete(STMT_LO{ 1, StatementType::All }), stmt2,
            PKBRelationship::FOLLOWS));

        REQUIRE_FALSE(pkb->isRelationshipPresent(stmt5, stmt6, PKBRelationship::FOLLOWS));
        REQUIRE_FALSE(pkb->isRelationshipPresent(stmt8, stmt9, PKBRelationship::FOLLOWS));
        REQUIRE_FALSE(pkb->isRelationshipPresent(Example, stmt9, PKBRelationship::FOLLOWS));

        // Concrete statements with wrong types will be caught by PKB::validateStatement
        REQUIRE_FALSE(pkb->isRelationshipPresent(PKBField::createConcrete(STMT_LO{ 1, StatementType::Read }), stmt2,
            PKBRelationship::FOLLOWS));

        // Invalid 
        REQUIRE_FALSE(pkb->isRelationshipPresent(PKBField::createWildcard(PKBEntityType::STATEMENT),
            PKBField::createWildcard(PKBEntityType::VARIABLE), PKBRelationship::MODIFIES));
        REQUIRE_FALSE(pkb->isRelationshipPresent(PKBField::createWildcard(PKBEntityType::PROCEDURE),
            PKBField::createWildcard(PKBEntityType::VARIABLE), PKBRelationship::MODIFIES));
        REQUIRE_FALSE(pkb->isRelationshipPresent(PKBField::createWildcard(PKBEntityType::PROCEDURE),
            PKBField::createWildcard(PKBEntityType::VARIABLE), PKBRelationship::FOLLOWS));
    }

    SECTION("PKB::getRelationship") {
        using FieldRowResponse = std::unordered_set<std::vector<PKBField>, PKBFieldVectorHash>;

        // Follows(_, _)
        REQUIRE(pkb->getRelationship(PKBField::createWildcard(PKBEntityType::STATEMENT),
            PKBField::createWildcard(PKBEntityType::STATEMENT), PKBRelationship::FOLLOWS) == PKBResponse{ true,
            FieldRowResponse{ {stmt1, stmt2}, {stmt2, stmt3}, {stmt3, stmt4}, {stmt4, stmt5}, {stmt5, stmt12},
            {stmt6, stmt7}, {stmt7, stmt10}, {stmt10, stmt11}, {stmt12, stmt13} } });

        // Follows*(_, _)
        REQUIRE(pkb->getRelationship(PKBField::createWildcard(PKBEntityType::STATEMENT),
            PKBField::createWildcard(PKBEntityType::STATEMENT), PKBRelationship::FOLLOWST) == PKBResponse{ true,
            FieldRowResponse{ {stmt1, stmt2}, {stmt1, stmt3}, {stmt1, stmt4}, {stmt1, stmt5}, {stmt1, stmt12},
            {stmt1, stmt13}, {stmt2, stmt3}, {stmt2, stmt4}, {stmt2, stmt5}, {stmt2, stmt12}, {stmt2, stmt13},
            {stmt3, stmt4}, {stmt3, stmt5}, {stmt3, stmt12}, {stmt3, stmt13}, {stmt4, stmt5}, {stmt4, stmt12},
            {stmt4, stmt13}, {stmt5, stmt12}, {stmt5, stmt13}, {stmt6, stmt7}, {stmt6, stmt10}, {stmt6, stmt11},
            {stmt7, stmt10}, {stmt7, stmt11}, {stmt10, stmt11}, {stmt12, stmt13} } });

        // Follows*(s, _)
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(StatementType::All),
            PKBField::createWildcard(PKBEntityType::STATEMENT), PKBRelationship::FOLLOWST) == PKBResponse{ true,
            FieldRowResponse{ {stmt1, stmt2}, {stmt1, stmt3}, {stmt1, stmt4}, {stmt1, stmt5}, {stmt1, stmt12},
            {stmt1, stmt13}, {stmt2, stmt3}, {stmt2, stmt4}, {stmt2, stmt5}, {stmt2, stmt12}, {stmt2, stmt13},
            {stmt3, stmt4}, {stmt3, stmt5}, {stmt3, stmt12}, {stmt3, stmt13}, {stmt4, stmt5}, {stmt4, stmt12},
            {stmt4, stmt13}, {stmt5, stmt12}, {stmt5, stmt13}, {stmt6, stmt7}, {stmt6, stmt10}, {stmt6, stmt11},
            {stmt7, stmt10}, {stmt7, stmt11}, {stmt10, stmt11}, {stmt12, stmt13} } });

        // Follows*(_, s)
        REQUIRE(pkb->getRelationship(PKBField::createWildcard(PKBEntityType::STATEMENT),
            PKBField::createDeclaration(StatementType::All), PKBRelationship::FOLLOWST) == PKBResponse{ true,
            FieldRowResponse{ {stmt1, stmt2}, {stmt1, stmt3}, {stmt1, stmt4}, {stmt1, stmt5}, {stmt1, stmt12},
            {stmt1, stmt13}, {stmt2, stmt3}, {stmt2, stmt4}, {stmt2, stmt5}, {stmt2, stmt12}, {stmt2, stmt13},
            {stmt3, stmt4}, {stmt3, stmt5}, {stmt3, stmt12}, {stmt3, stmt13}, {stmt4, stmt5}, {stmt4, stmt12},
            {stmt4, stmt13}, {stmt5, stmt12}, {stmt5, stmt13}, {stmt6, stmt7}, {stmt6, stmt10}, {stmt6, stmt11},
            {stmt7, stmt10}, {stmt7, stmt11}, {stmt10, stmt11}, {stmt12, stmt13} } });

        // Follows*(a, _)
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(StatementType::Assignment),
            PKBField::createWildcard(PKBEntityType::STATEMENT), PKBRelationship::FOLLOWST) == PKBResponse{ true,
            FieldRowResponse{ {stmt1, stmt2}, {stmt1, stmt3}, {stmt1, stmt4}, {stmt1, stmt5}, {stmt1, stmt12},
            {stmt1, stmt13}, {stmt2, stmt3}, {stmt2, stmt4}, {stmt2, stmt5}, {stmt2, stmt12}, {stmt2, stmt13},
            {stmt3, stmt4}, {stmt3, stmt5}, {stmt3, stmt12}, {stmt3, stmt13}, {stmt6, stmt7}, {stmt6, stmt10},
            {stmt6, stmt11}, {stmt10, stmt11} } });

        // Follows*(_, a)
        REQUIRE(pkb->getRelationship(PKBField::createWildcard(PKBEntityType::STATEMENT),
            PKBField::createDeclaration(StatementType::Assignment), PKBRelationship::FOLLOWST) == PKBResponse{ true,
            FieldRowResponse{ {stmt1, stmt2}, {stmt1, stmt3}, {stmt2, stmt3}, {stmt6, stmt10},
            {stmt6, stmt11}, {stmt7, stmt10}, {stmt7, stmt11}, {stmt10, stmt11} } });

        // Follows*(1, _)
        REQUIRE(pkb->getRelationship(PKBField::createConcrete(STMT_LO{ 1, StatementType::Assignment }),
            PKBField::createWildcard(PKBEntityType::STATEMENT), PKBRelationship::FOLLOWST) == PKBResponse{ true,
            FieldRowResponse{ {stmt1, stmt2}, {stmt1, stmt3}, {stmt1, stmt4}, {stmt1, stmt5}, {stmt1, stmt12},
            {stmt1, stmt13} } });

        // Follows*(1, _) without statement type
        REQUIRE(pkb->getRelationship(PKBField::createConcrete(STMT_LO{ 1 }),
            PKBField::createWildcard(PKBEntityType::STATEMENT), PKBRelationship::FOLLOWST) == PKBResponse{ true,
            FieldRowResponse{ {stmt1, stmt2}, {stmt1, stmt3}, {stmt1, stmt4}, {stmt1, stmt5}, {stmt1, stmt12},
            {stmt1, stmt13} } });

        // Follows*(1, 2) 
        REQUIRE(pkb->getRelationship(PKBField::createConcrete(STMT_LO{ 1 }),
            PKBField::createConcrete(STMT_LO{ 2 }), PKBRelationship::FOLLOWST) == PKBResponse{ true,
            FieldRowResponse{ {stmt1, stmt2} } });

        // Follows(1, 2) with wrong statement type
        REQUIRE(pkb->getRelationship(PKBField::createConcrete(STMT_LO{ 1, StatementType::While }),
            PKBField::createConcrete(STMT_LO{ 2 }), PKBRelationship::FOLLOWS) == PKBResponse{ false,
            FieldRowResponse{ } });

        // Follows*(1, 2) with wrong statement type
        REQUIRE(pkb->getRelationship(PKBField::createConcrete(STMT_LO{ 1, StatementType::While }),
            PKBField::createConcrete(STMT_LO{ 2 }), PKBRelationship::FOLLOWST) == PKBResponse{ false,
            FieldRowResponse{ } });

        // Follows(p, _) invalid parameter
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(PKBEntityType::PROCEDURE),
            PKBField::createWildcard(PKBEntityType::STATEMENT), PKBRelationship::FOLLOWS) == PKBResponse{ false,
            FieldRowResponse{ } });

        // Follows(16, _) invalid statement
        REQUIRE(pkb->getRelationship(PKBField::createConcrete(STMT_LO{ 16 }),
            PKBField::createWildcard(PKBEntityType::STATEMENT), PKBRelationship::FOLLOWS) == PKBResponse{ false,
            FieldRowResponse{ } });

        // Modifies(_, _) 
        REQUIRE(pkb->getRelationship(PKBField::createWildcard(PKBEntityType::STATEMENT),
            PKBField::createWildcard(PKBEntityType::VARIABLE), PKBRelationship::MODIFIES) == PKBResponse{ false,
            FieldRowResponse{ } });

        // Modifies(_, _) 
        REQUIRE(pkb->getRelationship(PKBField::createWildcard(PKBEntityType::PROCEDURE),
            PKBField::createWildcard(PKBEntityType::VARIABLE), PKBRelationship::MODIFIES) == PKBResponse{ false,
            FieldRowResponse{ } });

        // Modifies(p, _) 
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(PKBEntityType::PROCEDURE),
            PKBField::createWildcard(PKBEntityType::VARIABLE), PKBRelationship::MODIFIES) == PKBResponse{ true,
            FieldRowResponse{ {Example, x}, {Example, y}, {Example, z}, {Example, i}, {Example, monke}, {Example, w},
            {abc, w}, {def, w}} });

        // Modifies(w, _) 
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(StatementType::While),
            PKBField::createWildcard(PKBEntityType::VARIABLE), PKBRelationship::MODIFIES) == PKBResponse{ true,
            FieldRowResponse{ {stmt5, x}, {stmt5, z}, {stmt5, y}, {stmt5, i} } });

        // Modifies(10, _) 
        REQUIRE(pkb->getRelationship(PKBField::createConcrete(STMT_LO{ 10 }),
            PKBField::createWildcard(PKBEntityType::VARIABLE), PKBRelationship::MODIFIES) == PKBResponse{ true,
            FieldRowResponse{ {stmt10, z} } });

        // Modifies(10, _) 
        REQUIRE(pkb->getRelationship(PKBField::createConcrete(STMT_LO{ 10, StatementType::All }),
            PKBField::createWildcard(PKBEntityType::VARIABLE), PKBRelationship::MODIFIES) == PKBResponse{ true,
            FieldRowResponse{ {stmt10, z} } });

        // Modifies(s, _) 
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(StatementType::All),
            PKBField::createWildcard(PKBEntityType::VARIABLE), PKBRelationship::MODIFIES) == PKBResponse{ true,
            FieldRowResponse{ {stmt1, x}, {stmt2, z}, {stmt3, i}, {stmt4, monke}, {stmt5, x}, {stmt5, z}, {stmt5, y},
            {stmt5, i}, {stmt6, x}, {stmt7, z}, {stmt7, y}, {stmt8, z}, {stmt9, y}, {stmt10, z}, {stmt11, i} } });

        // Modifies(s, v) 
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(StatementType::All),
            PKBField::createDeclaration(PKBEntityType::VARIABLE), PKBRelationship::MODIFIES) == PKBResponse{ true,
            FieldRowResponse{ {stmt1, x}, {stmt2, z}, {stmt3, i}, {stmt4, monke}, {stmt5, x}, {stmt5, z}, {stmt5, y},
            {stmt5, i}, {stmt6, x}, {stmt7, z}, {stmt7, y}, {stmt8, z}, {stmt9, y}, {stmt10, z}, {stmt11, i} } });

        // Modifies(s, "z")
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(StatementType::All), z,
            PKBRelationship::MODIFIES) == PKBResponse{ true,
            FieldRowResponse{ {stmt2, z}, {stmt5, z}, {stmt7, z}, {stmt8, z}, {stmt10, z} } });

        // Modifies(a, _) 
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(StatementType::Assignment),
            PKBField::createWildcard(PKBEntityType::VARIABLE), PKBRelationship::MODIFIES) == PKBResponse{ true,
            FieldRowResponse{ {stmt1, x}, {stmt2, z}, {stmt3, i}, {stmt6, x}, {stmt8, z},
            {stmt9, y}, {stmt10, z}, {stmt11, i} } });

        // Modifies(r, _) 
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(StatementType::Read),
            PKBField::createWildcard(PKBEntityType::VARIABLE), PKBRelationship::MODIFIES) == PKBResponse{ true,
            FieldRowResponse{ {stmt4, monke} } });

        // Modifies(_, p) invalid parameter
        REQUIRE(pkb->getRelationship(PKBField::createWildcard(PKBEntityType::STATEMENT),
            PKBField::createDeclaration(PKBEntityType::PROCEDURE), PKBRelationship::MODIFIES) == PKBResponse{ false,
            FieldRowResponse{ } });

        // Modifies(v, _) invalid parameter
        REQUIRE(pkb->getRelationship(PKBField::createWildcard(PKBEntityType::VARIABLE),
            PKBField::createWildcard(PKBEntityType::STATEMENT), PKBRelationship::MODIFIES) == PKBResponse{ false,
            FieldRowResponse{ } });

        // Uses(_, _) 
        REQUIRE(pkb->getRelationship(PKBField::createWildcard(PKBEntityType::STATEMENT),
            PKBField::createWildcard(PKBEntityType::VARIABLE), PKBRelationship::USES) == PKBResponse{ false,
            FieldRowResponse{ } });

        // Uses(_, _) 
        REQUIRE(pkb->getRelationship(PKBField::createWildcard(PKBEntityType::PROCEDURE),
            PKBField::createWildcard(PKBEntityType::VARIABLE), PKBRelationship::USES) == PKBResponse{ false,
            FieldRowResponse{ } });

        // Uses(p, _) 
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(PKBEntityType::PROCEDURE),
            PKBField::createWildcard(PKBEntityType::VARIABLE), PKBRelationship::USES) == PKBResponse{ true,
            FieldRowResponse{ {Example, x}, {Example, z}, {Example, i}, {Example, monke} } });

        // Uses(w, _) 
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(StatementType::While),
            PKBField::createWildcard(PKBEntityType::VARIABLE), PKBRelationship::USES) == PKBResponse{ true,
            FieldRowResponse{ {stmt5, i}, {stmt5, x}, {stmt5, z} } });

        // Uses(i, _) 
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(StatementType::If),
            PKBField::createWildcard(PKBEntityType::VARIABLE), PKBRelationship::USES) == PKBResponse{ true,
            FieldRowResponse{ {stmt7, x}, {stmt7, z} } });

        // Uses(10, _) 
        REQUIRE(pkb->getRelationship(PKBField::createConcrete(STMT_LO{ 10 }),
            PKBField::createWildcard(PKBEntityType::VARIABLE), PKBRelationship::USES) == PKBResponse{ true,
            FieldRowResponse{ {stmt10, z}, {stmt10, x}, {stmt10, i} } });

        // Uses(10, _) 
        REQUIRE(pkb->getRelationship(PKBField::createConcrete(STMT_LO{ 10, StatementType::Assignment }),
            PKBField::createWildcard(PKBEntityType::VARIABLE), PKBRelationship::USES) == PKBResponse{ true,
            FieldRowResponse{ {stmt10, z}, {stmt10, x}, {stmt10, i} } });

        // Uses(10, _) 
        REQUIRE(pkb->getRelationship(PKBField::createConcrete(STMT_LO{ 10, StatementType::All }),
            PKBField::createWildcard(PKBEntityType::VARIABLE), PKBRelationship::USES) == PKBResponse{ true,
            FieldRowResponse{ {stmt10, z}, {stmt10, x}, {stmt10, i} } });

        // Uses(p, _) 
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(StatementType::Print),
            PKBField::createWildcard(PKBEntityType::VARIABLE), PKBRelationship::USES) == PKBResponse{ true,
            FieldRowResponse{ {stmt12, monke} } });

        // Uses(s, _) 
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(StatementType::All),
            PKBField::createWildcard(PKBEntityType::VARIABLE), PKBRelationship::USES) == PKBResponse{ true,
            FieldRowResponse{ {stmt5, i}, {stmt5, x}, {stmt5, z}, {stmt6, x}, {stmt7, x}, {stmt7, z}, {stmt8, x},
            {stmt9, z}, {stmt9, x}, {stmt10, z}, {stmt10, x}, {stmt10, i}, {stmt11, i}, {stmt12, monke} } });

        // Uses(s, v) 
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(StatementType::All),
            PKBField::createDeclaration(PKBEntityType::VARIABLE), PKBRelationship::USES) == PKBResponse{ true,
            FieldRowResponse{ {stmt5, i}, {stmt5, x}, {stmt5, z}, {stmt6, x}, {stmt7, x}, {stmt7, z}, {stmt8, x},
            {stmt9, z}, {stmt9, x}, {stmt10, z}, {stmt10, x}, {stmt10, i}, {stmt11, i}, {stmt12, monke} } });

        // Uses(s, "z")
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(StatementType::All), z,
            PKBRelationship::USES) == PKBResponse{ true,
            FieldRowResponse{ {stmt5, z}, {stmt7, z}, {stmt9, z}, {stmt10, z} } });

        // Uses(a, _) 
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(StatementType::Assignment),
            PKBField::createWildcard(PKBEntityType::VARIABLE), PKBRelationship::USES) == PKBResponse{ true,
            FieldRowResponse{ {stmt6, x}, {stmt8, x}, {stmt9, z}, {stmt9, x}, {stmt10, z}, {stmt10, x},
            {stmt10, i}, {stmt11, i} } });

        // Uses(_, p) invalid parameter
        REQUIRE(pkb->getRelationship(PKBField::createWildcard(PKBEntityType::STATEMENT),
            PKBField::createDeclaration(PKBEntityType::PROCEDURE), PKBRelationship::USES) == PKBResponse{ false,
            FieldRowResponse{ } });

        // Uses(v, _) invalid parameter
        REQUIRE(pkb->getRelationship(PKBField::createWildcard(PKBEntityType::VARIABLE),
            PKBField::createWildcard(PKBEntityType::STATEMENT), PKBRelationship::USES) == PKBResponse{ false,
            FieldRowResponse{ } });

        // Parent(_, _)
        REQUIRE(pkb->getRelationship(PKBField::createWildcard(PKBEntityType::STATEMENT),
            PKBField::createWildcard(PKBEntityType::STATEMENT), PKBRelationship::PARENT) == PKBResponse{ true,
            FieldRowResponse{ {stmt5, stmt6}, {stmt5, stmt7}, {stmt7, stmt8}, {stmt7, stmt9}, {stmt5, stmt10},
            {stmt5, stmt11}} });

        // Parent(s, s1)
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(StatementType::All),
            PKBField::createDeclaration(StatementType::All), PKBRelationship::PARENT) == PKBResponse{ true,
            FieldRowResponse{ {stmt5, stmt6}, {stmt5, stmt7}, {stmt7, stmt8}, {stmt7, stmt9}, {stmt5, stmt10},
            {stmt5, stmt11}} });

        // Parent(5, s)
        REQUIRE(pkb->getRelationship(PKBField::createConcrete(STMT_LO{ 5, StatementType::All }),
            PKBField::createDeclaration(StatementType::All), PKBRelationship::PARENT) == PKBResponse{ true,
            FieldRowResponse{ {stmt5, stmt6}, {stmt5, stmt7}, {stmt5, stmt10}, {stmt5, stmt11}} });

        // Parent(5, s)
        REQUIRE(pkb->getRelationship(PKBField::createConcrete(STMT_LO{ 5, StatementType::While }),
            PKBField::createDeclaration(StatementType::All), PKBRelationship::PARENT) == PKBResponse{ true,
            FieldRowResponse{ {stmt5, stmt6}, {stmt5, stmt7}, {stmt5, stmt10}, {stmt5, stmt11}} });

        // Parent(5, s)
        REQUIRE(pkb->getRelationship(PKBField::createConcrete(STMT_LO{ 5 }),
            PKBField::createDeclaration(StatementType::All), PKBRelationship::PARENT) == PKBResponse{ true,
            FieldRowResponse{ {stmt5, stmt6}, {stmt5, stmt7}, {stmt5, stmt10}, {stmt5, stmt11}} });

        // Parent(5, r)
        REQUIRE(pkb->getRelationship(PKBField::createConcrete(STMT_LO{ 5 }),
            PKBField::createDeclaration(StatementType::Read), PKBRelationship::PARENT) == PKBResponse{ false,
            FieldRowResponse{ } });

        // Parent(s, 6)
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(StatementType::All),
            PKBField::createConcrete(STMT_LO{ 6 }), PKBRelationship::PARENT) == PKBResponse{ true,
            FieldRowResponse{ {stmt5, stmt6} } });

        // Parent(s, 6)
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(StatementType::All),
            PKBField::createConcrete(STMT_LO{ 6, StatementType::All }), PKBRelationship::PARENT) == PKBResponse{ true,
            FieldRowResponse{ {stmt5, stmt6} } });

        // Parent(s, 6)
        REQUIRE(
            pkb->getRelationship(
                PKBField::createDeclaration(StatementType::All),
                PKBField::createConcrete(STMT_LO{ 6, StatementType::Assignment }),
                PKBRelationship::PARENT
            ) == PKBResponse{ true, FieldRowResponse{ {stmt5, stmt6} } }
        );

        // Parent(5, 6)
        REQUIRE(pkb->getRelationship(PKBField::createConcrete(STMT_LO{ 5 }),
            PKBField::createConcrete(STMT_LO{ 6 }), PKBRelationship::PARENT) == PKBResponse{ true,
            FieldRowResponse{ {stmt5, stmt6} } });

        // Parent*(_, _)
        REQUIRE(pkb->getRelationship(PKBField::createWildcard(PKBEntityType::STATEMENT),
            PKBField::createWildcard(PKBEntityType::STATEMENT), PKBRelationship::PARENTT) == PKBResponse{ true,
            FieldRowResponse{ {stmt5, stmt6}, {stmt5, stmt7}, {stmt5, stmt8}, {stmt5, stmt9}, {stmt7, stmt8},
            {stmt7, stmt9}, {stmt5, stmt10}, {stmt5, stmt11}} });

        // Parent*(5, _)
        REQUIRE(pkb->getRelationship(PKBField::createConcrete(STMT_LO{ 5 }),
            PKBField::createWildcard(PKBEntityType::STATEMENT), PKBRelationship::PARENTT) == PKBResponse{ true,
            FieldRowResponse{ {stmt5, stmt6}, {stmt5, stmt7}, {stmt5, stmt8}, {stmt5, stmt9},
            {stmt5, stmt10}, {stmt5, stmt11}} });

        // Parent*(5, a)
        REQUIRE(pkb->getRelationship(PKBField::createConcrete(STMT_LO{ 5 }),
            PKBField::createDeclaration(StatementType::Assignment), PKBRelationship::PARENTT) == PKBResponse{ true,
            FieldRowResponse{ {stmt5, stmt6}, {stmt5, stmt8}, {stmt5, stmt9},
            {stmt5, stmt10}, {stmt5, stmt11}} });

        // Parent*(5, a)
        REQUIRE(pkb->getRelationship(PKBField::createConcrete(STMT_LO{ 5, StatementType::All }),
            PKBField::createDeclaration(StatementType::Assignment), PKBRelationship::PARENTT) == PKBResponse{ true,
            FieldRowResponse{ {stmt5, stmt6}, {stmt5, stmt8}, {stmt5, stmt9},
            {stmt5, stmt10}, {stmt5, stmt11}} });

        // Parent*(5, i)
        REQUIRE(pkb->getRelationship(PKBField::createConcrete(STMT_LO{ 5, StatementType::All }),
            PKBField::createDeclaration(StatementType::If), PKBRelationship::PARENTT) == PKBResponse{ true,
            FieldRowResponse{ {stmt5, stmt7} } });

        // Parent*(5, 5)
        REQUIRE(pkb->getRelationship(PKBField::createConcrete(STMT_LO{ 5, StatementType::All }),
            PKBField::createConcrete(STMT_LO{ 5, StatementType::All }), PKBRelationship::PARENTT) == PKBResponse{ false,
            FieldRowResponse{ } });

        // Parent*(5, 8)
        REQUIRE(pkb->getRelationship(PKBField::createConcrete(STMT_LO{ 5, StatementType::All }),
            PKBField::createConcrete(STMT_LO{ 8, StatementType::All }), PKBRelationship::PARENTT) == PKBResponse{ true,
            FieldRowResponse{ {stmt5, stmt8} } });

        // Parents*(p, _) invalid parameter
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(PKBEntityType::PROCEDURE),
            PKBField::createWildcard(PKBEntityType::STATEMENT), PKBRelationship::PARENTT) == PKBResponse{ false,
            FieldRowResponse{ } });

        // Parents*(_, v) invalid parameter
        REQUIRE(pkb->getRelationship(PKBField::createWildcard(PKBEntityType::STATEMENT),
            PKBField::createDeclaration(PKBEntityType::VARIABLE), PKBRelationship::PARENTT) == PKBResponse{ false,
            FieldRowResponse{ } });

        // Calls(_, _)
        REQUIRE(pkb->getRelationship(PKBField::createWildcard(PKBEntityType::PROCEDURE),
            PKBField::createWildcard(PKBEntityType::PROCEDURE), PKBRelationship::CALLS) == PKBResponse{ true,
            FieldRowResponse{ {Example, abc}, {abc, def} } });

        // Calls(p, _)
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(PKBEntityType::PROCEDURE),
            PKBField::createWildcard(PKBEntityType::PROCEDURE), PKBRelationship::CALLS) == PKBResponse{ true,
            FieldRowResponse{ {Example, abc}, {abc, def} } });

        // Calls(_, p)
        REQUIRE(pkb->getRelationship(PKBField::createWildcard(PKBEntityType::PROCEDURE),
            PKBField::createDeclaration(PKBEntityType::PROCEDURE), PKBRelationship::CALLS) == PKBResponse{ true,
            FieldRowResponse{ {Example, abc}, {abc, def} } });

        // Calls(Example, p)
        REQUIRE(pkb->getRelationship(Example, PKBField::createDeclaration(PKBEntityType::PROCEDURE),
            PKBRelationship::CALLS) == PKBResponse{ true, FieldRowResponse{ {Example, abc} } });

        // Calls*(_, _)
        REQUIRE(pkb->getRelationship(PKBField::createWildcard(PKBEntityType::PROCEDURE),
            PKBField::createWildcard(PKBEntityType::PROCEDURE), PKBRelationship::CALLST) == PKBResponse{ true,
            FieldRowResponse{ {Example, abc}, {Example, def}, {abc, def} } });

        // Calls*(p, _)
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(PKBEntityType::PROCEDURE),
            PKBField::createWildcard(PKBEntityType::PROCEDURE), PKBRelationship::CALLST) == PKBResponse{ true,
            FieldRowResponse{ {Example, abc}, {Example, def}, {abc, def} } });

        // Calls*(p, def)
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(PKBEntityType::PROCEDURE), def,
            PKBRelationship::CALLST) == PKBResponse{ true,
            FieldRowResponse{ {Example, def}, {abc, def} } });

        // Calls*(Example, _)
        REQUIRE(pkb->getRelationship(Example, PKBField::createDeclaration(PKBEntityType::PROCEDURE),
            PKBRelationship::CALLST) == PKBResponse{ true, FieldRowResponse{ {Example, abc}, {Example, def} } });

        // Calls*(def, _)
        REQUIRE(pkb->getRelationship(def, PKBField::createDeclaration(PKBEntityType::PROCEDURE),
            PKBRelationship::CALLST) == PKBResponse{ false, FieldRowResponse{ } });

        // Next(_,_) double wildcards
        REQUIRE(pkb->getRelationship(PKBField::createWildcard(PKBEntityType::STATEMENT),
            PKBField::createWildcard(PKBEntityType::STATEMENT), PKBRelationship::NEXT) == PKBResponse{ true,
            FieldRowResponse{ {stmt1, stmt2}, {stmt2, stmt3}, {stmt3, stmt4}, {stmt4, stmt5}, {stmt5, stmt6},
            {stmt6, stmt7}, {stmt7, stmt8}, {stmt7, stmt9}, {stmt8, stmt10}, {stmt9, stmt10}, {stmt10, stmt11},
            {stmt11, stmt5}, {stmt11, stmt12}, {stmt12, stmt13}, {stmt5, stmt12} } });

        // Next(s1, s2) double statement decl
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(StatementType::All),
            PKBField::createDeclaration(StatementType::All), PKBRelationship::NEXT) == PKBResponse{ true,
            FieldRowResponse{ {stmt1, stmt2}, {stmt2, stmt3}, {stmt3, stmt4}, {stmt4, stmt5}, {stmt5, stmt6},
            {stmt5, stmt12}, {stmt6, stmt7}, {stmt7, stmt8}, {stmt7, stmt9}, {stmt8, stmt10}, {stmt9, stmt10},
            {stmt10, stmt11}, {stmt11, stmt5}, {stmt11, stmt12}, {stmt12, stmt13} } });

        // Next(11, 5) douuble concrete decl
        REQUIRE(pkb->getRelationship(PKBField::createConcrete(Content{ STMT_LO{11, StatementType::Assignment} }),
            PKBField::createConcrete(Content{ STMT_LO{ 5, StatementType::While } }), PKBRelationship::NEXT) ==
            PKBResponse{ true, FieldRowResponse{ {stmt11, stmt5} } });

        // Next(7, a) first concrete, second assn decl
        REQUIRE(pkb->getRelationship(PKBField::createConcrete(Content{ STMT_LO{7, StatementType::If } }),
            PKBField::createDeclaration(StatementType::Assignment), PKBRelationship::NEXT) == PKBResponse{ true,
            FieldRowResponse{ {stmt7, stmt8}, {stmt7, stmt9} } });

        // Next(a, 5) first assn decl, second concrete
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(StatementType::Assignment),
            PKBField::createConcrete(STMT_LO{ 5, StatementType::While }), PKBRelationship::NEXT) == PKBResponse{ true,
            FieldRowResponse{ {stmt11, stmt5} } });

        // Next*(ifs, a) first if decl, second assn decl
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(StatementType::If),
            PKBField::createDeclaration(StatementType::Assignment), PKBRelationship::NEXTT) == PKBResponse{ true,
            FieldRowResponse{ {stmt7, stmt8}, {stmt7, stmt9}, {stmt7, stmt10}, {stmt7, stmt11}, {stmt7, stmt6} } });

        // Next*(5, a) first concrete, second assn decl
        REQUIRE(pkb->getRelationship(PKBField::createConcrete(STMT_LO{ 5, StatementType::While }),
            PKBField::createDeclaration(StatementType::Assignment), PKBRelationship::NEXTT) == PKBResponse{ true,
            FieldRowResponse{ {stmt5, stmt6}, {stmt5, stmt8}, {stmt5, stmt9}, {stmt5, stmt10}, {stmt5, stmt11} } });

        // Next*(a, 7) first assn decl, second concrete
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(StatementType::Assignment),
            PKBField::createConcrete(STMT_LO{ 7, StatementType::If }), PKBRelationship::NEXTT) == PKBResponse{ true,
            FieldRowResponse{ {stmt1, stmt7}, {stmt2, stmt7}, {stmt3, stmt7}, {stmt6, stmt7}, {stmt8, stmt7},
            {stmt9, stmt7}, {stmt10, stmt7}, {stmt11, stmt7} } });

        // Next*(ifs1, ifs2) test if traversal can handle insertion of initial node i.e. (7, 7)
        REQUIRE(pkb->getRelationship(PKBField::createDeclaration(StatementType::If),
            PKBField::createDeclaration(StatementType::If), PKBRelationship::NEXTT) == PKBResponse{ true,
            FieldRowResponse{ {stmt7, stmt7} } });
    }
}

TEST_CASE("PKB Affects Testing - Single Proc") {
    auto testCode = R"(procedure main {
    x = 2 + a;
    y = x + 3;
    if (b == 3) then {
        while (x == 2) {
            x = x + 1;
        }
    } else {
        a = x + c;
    }
    print x;
    a = x - a;
    y = a + x;
    })";
    auto cfgExtractor = sp::cfg::CFGExtractor();
    auto cfgContainer = cfgExtractor.extract(sp::parser::parse(testCode).get());
    std::unique_ptr<PKB> pkb = std::make_unique<PKB>();
    pkb->insertCFG(cfgContainer);
    
    StatementType ASSIGN = StatementType::Assignment;
    StatementType IF = StatementType::If;
    StatementType WHILE = StatementType::While;
    StatementType PRINT = StatementType::Print;

    // Insert statements into stmtTable
    pkb->insertEntity(STMT_LO(1, ASSIGN));
    pkb->insertEntity(STMT_LO(2, ASSIGN));
    pkb->insertEntity(STMT_LO(3, IF));
    pkb->insertEntity(STMT_LO(4, WHILE));
    pkb->insertEntity(STMT_LO(5, ASSIGN));
    pkb->insertEntity(STMT_LO(6, ASSIGN));
    pkb->insertEntity(STMT_LO(7, PRINT));
    pkb->insertEntity(STMT_LO(8, ASSIGN));
    pkb->insertEntity(STMT_LO(9, ASSIGN));

    PKBRelationship aff = PKBRelationship::AFFECTS;
    PKBRelationship affT = PKBRelationship::AFFECTST;

    SECTION("AffectsEvaluator::contains (transitive and non-transitive)") {
        PKBField conc1 = PKBField::createConcrete(STMT_LO(1, ASSIGN));
        PKBField conc2 = PKBField::createConcrete(STMT_LO(8, ASSIGN));
        REQUIRE(pkb->isRelationshipPresent(conc1, conc2, aff));

        PKBField conc3 = PKBField::createConcrete(STMT_LO(5, ASSIGN));
        PKBField conc4 = PKBField::createConcrete(STMT_LO(5, ASSIGN));
        REQUIRE(pkb->isRelationshipPresent(conc3, conc4, aff));

        PKBField conc5 = PKBField::createConcrete(STMT_LO(2, ASSIGN));
        REQUIRE_FALSE(pkb->isRelationshipPresent(conc5, conc3, aff));

        PKBField conc6 = PKBField::createConcrete(STMT_LO(9, ASSIGN));
        REQUIRE(pkb->isRelationshipPresent(conc1, conc6, affT));
        REQUIRE(pkb->isRelationshipPresent(conc1, conc6, aff));
    }

    SECTION("AffectsEvaluator::retrieve (non-transitive)") {
        PKBField stmtDecl = PKBField::createDeclaration(StatementType::All);

        PKBField conc1 = PKBField::createConcrete(STMT_LO(1, ASSIGN));
        PKBField conc2 = PKBField::createConcrete(STMT_LO(2, ASSIGN));
        PKBField conc5 = PKBField::createConcrete(STMT_LO(5, ASSIGN));
        PKBField conc6 = PKBField::createConcrete(STMT_LO(6, ASSIGN));
        PKBField conc8 = PKBField::createConcrete(STMT_LO(8, ASSIGN));
        PKBField conc9 = PKBField::createConcrete(STMT_LO(9, ASSIGN));

        // (Decl, Concrete)
        FieldRowResponse expected1{ {conc1, conc8}, {conc5, conc8}, {conc6, conc8} };
        REQUIRE(*(pkb->getRelationship(stmtDecl, conc8, aff)
            .getResponse<FieldRowResponse>()) == expected1);
        REQUIRE(!pkb->getRelationship(stmtDecl, conc1, aff).hasResult);

        // (Concrete, Decl)
        FieldRowResponse expected2{ {conc5, conc5}, {conc5, conc8}, {conc5, conc9} };
        FieldRowResponse expected3{ {conc1, conc2}, {conc1, conc5}, {conc1, conc6}, {conc1, conc8}, {conc1, conc9} };
        REQUIRE(*(pkb->getRelationship(conc5, stmtDecl, aff)
            .getResponse<FieldRowResponse>()) == expected2);
        REQUIRE(*(pkb->getRelationship(conc1, stmtDecl, aff)
            .getResponse<FieldRowResponse>()) == expected3);

        // (Decl, Decl)
        FieldRowResponse expected4{
            {conc1, conc2},
            {conc1, conc5},
            {conc1, conc6},
            {conc1, conc8},
            {conc1, conc9},
            {conc5, conc5},
            {conc5, conc8},
            {conc5, conc9},
            {conc6, conc8},
            {conc8, conc9}
        };
        REQUIRE(*(pkb->getRelationship(stmtDecl, stmtDecl, aff)
            .getResponse<FieldRowResponse>()) == expected4);
    }

    SECTION("AffectsEvaluator::retrieve (transitive)") {
        PKBField stmtDecl = PKBField::createDeclaration(StatementType::All);

        PKBField field1 = PKBField::createConcrete(STMT_LO(1, ASSIGN));
        PKBField field2 = PKBField::createConcrete(STMT_LO(2, ASSIGN));
        PKBField field3 = PKBField::createConcrete(STMT_LO(5, ASSIGN));
        PKBField field4 = PKBField::createConcrete(STMT_LO(6, ASSIGN));
        PKBField field5 = PKBField::createConcrete(STMT_LO(8, ASSIGN));
        PKBField field6 = PKBField::createConcrete(STMT_LO(9, ASSIGN));

        // (Decl, Concrete)
        FieldRowResponse expected1{ {field1, field6}, {field3, field6},
            {field4, field6}, {field5, field6} };
        REQUIRE(*(pkb->getRelationship(stmtDecl, field6, affT)
            .getResponse<FieldRowResponse>()) == expected1);

        // (Concrete, Decl)
        FieldRowResponse expected2{ {field3, field3}, {field3, field5}, {field3, field6} };
        REQUIRE(*(pkb->getRelationship(field3, stmtDecl, affT)
            .getResponse<FieldRowResponse>()) == expected2);

        // (Decl, Decl)
        FieldRowResponse expected3{
            {field1, field2},
            {field1, field3},
            {field1, field4},
            {field1, field5},
            {field1, field6},
            {field3, field3},
            {field3, field5},
            {field3, field6},
            {field4, field5},
            {field4, field6},
            {field5, field6}
        };
        REQUIRE(*(pkb->getRelationship(stmtDecl, stmtDecl, affT)
            .getResponse<FieldRowResponse>()) == expected3);
    }
}

TEST_CASE("AffectsEvaluator multi-proc test") {
    /**
    *       procedure main {
    * 1         x = a + b;
    * 2         if (x > 0) then {
    * 3             call second;
    *           } else {
    * 4             while (b > c) {
    * 5                 call third;
    * 6                 a = x + a;
    *               }
    *           }
    * 7         x = x + a;          
    *       }
    * 
    *       procedure second {
    * 8         x = b;
    * 9         a = c;
    *       }
    * 
    *       procedure third {
    * 10        while (y == x) {
    * 11            x = x + y;
    * 12            if (c > b) then {
    * 13                a = 4 + c;
    *               } else {
    * 14                c = a + 4;
    *               }
    *           }
    *       }
    */
    std::string sourceCode = R"(procedure main {
        x = a + b;
        if (x > 0) then {
            call second;
        } else {
            while (b > c) {
                call third;
                a = x + a;
            }
        }
    }
    procedure second {
        x = b;
        while (a > b) {
            y = y + 1;
            if (c > 4) then {
                x = b + y;
            } else {
                b = y * 3;
            }
        }
    }
    procedure third {
        while (y == x) {
            x = x + y;
            if (c > b) then {
                b = 4 + c;
            } else {
                c = a + 4;
            }
        }
    })";
    sp::cfg::CFGExtractor extractor;
    auto pkb = std::make_unique<PKB>();
    auto cfg = extractor.extract(sp::parser::parse(sourceCode).get());
    pkb->insertCFG(cfg);

    // Statement Types
    StatementType ASSIGN = StatementType::Assignment;
    StatementType ALL = StatementType::All;
    StatementType CALL = StatementType::Call;
    StatementType IF = StatementType::If;
    StatementType WHILE = StatementType::While;

    // Insert statements to stmtTable
    pkb->insertEntity(STMT_LO(1, ASSIGN));
    pkb->insertEntity(STMT_LO(2, IF));
    pkb->insertEntity(STMT_LO(3, CALL));
    pkb->insertEntity(STMT_LO(4, WHILE));
    pkb->insertEntity(STMT_LO(5, CALL));
    pkb->insertEntity(STMT_LO(6, ASSIGN));
    pkb->insertEntity(STMT_LO(7, ASSIGN));
    pkb->insertEntity(STMT_LO(8, WHILE));
    pkb->insertEntity(STMT_LO(9, ASSIGN));
    pkb->insertEntity(STMT_LO(10, IF));
    pkb->insertEntity(STMT_LO(11, ASSIGN));
    pkb->insertEntity(STMT_LO(12, ASSIGN));
    pkb->insertEntity(STMT_LO(13, WHILE));
    pkb->insertEntity(STMT_LO(14, ASSIGN));
    pkb->insertEntity(STMT_LO(15, IF));
    pkb->insertEntity(STMT_LO(16, ASSIGN));
    pkb->insertEntity(STMT_LO(17, ASSIGN));

    // Declarations for test case usage
    PKBField stmtDecl = PKBField::createDeclaration(ALL);

    PKBField conc1 = PKBField::createConcrete(STMT_LO(1, ASSIGN));
    PKBField conc6 = PKBField::createConcrete(STMT_LO(6, ASSIGN));
    PKBField conc7 = PKBField::createConcrete(STMT_LO(7, ASSIGN));
    PKBField conc9 = PKBField::createConcrete(STMT_LO(9, ASSIGN));
    PKBField conc11 = PKBField::createConcrete(STMT_LO(11, ASSIGN));
    PKBField conc12 = PKBField::createConcrete(STMT_LO(12, ASSIGN));
    PKBField conc14 = PKBField::createConcrete(STMT_LO(14, ASSIGN));
    PKBField conc16 = PKBField::createConcrete(STMT_LO(16, ASSIGN));
    PKBField conc17 = PKBField::createConcrete(STMT_LO(17, ASSIGN));

    PKBRelationship aff = PKBRelationship::AFFECTS;
    PKBRelationship affT = PKBRelationship::AFFECTST;

    SECTION("AffectsEvaluator::contains") {
        REQUIRE(pkb->isRelationshipPresent(conc17, conc16, aff));
        REQUIRE(pkb->isRelationshipPresent(conc9, conc9, aff));
        REQUIRE_FALSE(pkb->isRelationshipPresent(conc1, conc6, aff));
        REQUIRE_FALSE(pkb->isRelationshipPresent(conc6, conc17, affT));
    }

    SECTION("AffectsEvaluator::retrieve") {
        // (Declaration, Concrete)
        FieldRowResponse expected1{ {conc9, conc11}, {conc12, conc11} };
        REQUIRE(*(pkb->getRelationship(stmtDecl, conc11, aff)
            .getResponse<FieldRowResponse>()) == expected1);
        REQUIRE(!pkb->getRelationship(stmtDecl, conc7, aff).hasResult);

        // (Concrete, Declaration)
        FieldRowResponse expected2{ {conc9, conc9}, {conc9, conc11}, {conc9, conc12} };
        REQUIRE(*(pkb->getRelationship(conc9, stmtDecl, aff)
            .getResponse<FieldRowResponse>()) == expected2);
        REQUIRE(!pkb->getRelationship(conc16, stmtDecl, aff).hasResult);

        // (Declaration, Declaration)
        FieldRowResponse expected3{
            {conc6, conc6},
            {conc9, conc9},
            {conc9, conc11},
            {conc9, conc12},
            {conc12, conc11},
            {conc14, conc14},
            {conc17, conc16}
        };
        REQUIRE(*(pkb->getRelationship(stmtDecl, stmtDecl, aff)
            .getResponse<FieldRowResponse>()) == expected3);
    }
}

TEST_CASE("PKB regression test") {
    SECTION("PKB regression test #140.1") {
        std::unique_ptr<PKB> pkb = std::make_unique<PKB>();
        pkb->getRelationship(
            PKBField::createDeclaration(StatementType::All),
            PKBField::createDeclaration(PKBEntityType::VARIABLE),
            PKBRelationship::USES
        );
    }
    SECTION("PKB regression test #140.2") {
        std::unique_ptr<PKB> pkb = std::make_unique<PKB>();
        pkb->insertRelationship(PKBRelationship::USES,
            PKBField::createConcrete(STMT_LO{ 1, StatementType::Print }),
            PKBField::createConcrete(VAR_NAME("x"))
        );
        pkb->getRelationship(
            PKBField::createDeclaration(StatementType::All),
            PKBField::createDeclaration(PKBEntityType::VARIABLE),
            PKBRelationship::USES
        );
    }

    SECTION("PKB regression test #142.1") {
        std::unique_ptr<PKB> pkb = std::make_unique<PKB>();
        pkb->insertRelationship(PKBRelationship::USES,
            PKBField::createConcrete(STMT_LO{ 1, StatementType::Print }),
            PKBField::createConcrete(VAR_NAME("x"))
        );
        pkb->insertRelationship(PKBRelationship::USES,
            PKBField::createConcrete(STMT_LO{ 2, StatementType::Assignment }),
            PKBField::createConcrete(VAR_NAME("y"))
        );
        pkb->insertRelationship(PKBRelationship::USES,
            PKBField::createConcrete(PROC_NAME{ "main" }),
            PKBField::createConcrete(VAR_NAME("y"))
        );
        REQUIRE_NOTHROW(pkb->getRelationship(
            PKBField::createDeclaration(StatementType::All),
            PKBField::createDeclaration(PKBEntityType::VARIABLE),
            PKBRelationship::USES
        ));
    }

    SECTION("PKB regression test #142.2") {
        std::unique_ptr<PKB> pkb = std::make_unique<PKB>();
        pkb->insertRelationship(PKBRelationship::MODIFIES,
            PKBField::createConcrete(STMT_LO{ 1, StatementType::Print }),
            PKBField::createConcrete(VAR_NAME("x"))
        );
        pkb->insertRelationship(PKBRelationship::MODIFIES,
            PKBField::createConcrete(STMT_LO{ 2, StatementType::Assignment }),
            PKBField::createConcrete(VAR_NAME("y"))
        );
        pkb->insertRelationship(PKBRelationship::MODIFIES,
            PKBField::createConcrete(PROC_NAME{ "main" }),
            PKBField::createConcrete(VAR_NAME("y"))
        );
        REQUIRE_NOTHROW(pkb->getRelationship(
            PKBField::createDeclaration(StatementType::All),
            PKBField::createDeclaration(PKBEntityType::VARIABLE),
            PKBRelationship::MODIFIES
        ));
    }

    SECTION("PKB regression test #148") {
        std::unique_ptr<PKB> pkb = std::make_unique<PKB>();
        pkb->insertEntity(STMT_LO{ 1, StatementType::Assignment });

        PKBField field1 = PKBField::createConcrete(STMT_LO{ 1, StatementType::Assignment });
        PKBField field2 = PKBField::createConcrete(STMT_LO{ 1 });
        PKBField field3 = PKBField::createConcrete(VAR_NAME{ "a" });

        // Insert fails because "a" is not in variable table
        pkb->insertRelationship(PKBRelationship::MODIFIES, field2, field3);
        pkb->insertRelationship(PKBRelationship::MODIFIES, field1, field3);

        // Returns false because "a" is not in variable table
        REQUIRE_FALSE(pkb->isRelationshipPresent(field2, field3, PKBRelationship::MODIFIES));
        REQUIRE_FALSE(pkb->isRelationshipPresent(field1, field3, PKBRelationship::MODIFIES));
        REQUIRE_FALSE(pkb->isRelationshipPresent(field2, field3, PKBRelationship::MODIFIES));

        pkb->insertEntity(VAR_NAME{ "a" });
        pkb->insertRelationship(PKBRelationship::MODIFIES, field2, field3);
        pkb->insertRelationship(PKBRelationship::MODIFIES, field1, field3);
        REQUIRE(pkb->isRelationshipPresent(field2, field3, PKBRelationship::MODIFIES));
        REQUIRE(pkb->isRelationshipPresent(field1, field3, PKBRelationship::MODIFIES));
        REQUIRE(pkb->isRelationshipPresent(field2, field3, PKBRelationship::MODIFIES));

        // statement not in the statement table and STMT_LO initialised without a type
        PKBField field4 = PKBField::createConcrete(STMT_LO{ 3 });
        REQUIRE_FALSE(pkb->isRelationshipPresent(field4, field3, PKBRelationship::MODIFIES));
    }
}
